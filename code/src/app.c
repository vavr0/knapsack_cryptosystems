#include "app.h"
#include "bench.h"
#include "cli.h"
#include "system.h"
#include "utils.h"
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_MESSAGE_BITS 128

void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s demo  [options]\n", prog);
    fprintf(stderr, "  %s bench [options]\n", prog);
}

static size_t read_message_bits(i32 **message_out) {
    char line[256];

    printf("Enter plaintext bits (e.g. 101010): ");
    if (!fgets(line, sizeof(line), stdin)) {
        return 0;
    }

    size_t len = strcspn(line, "\r\n");
    line[len] = '\0';

    if (len == 0 || len > MAX_MESSAGE_BITS) {
        fprintf(stderr, "Invalid length. Use 1..%d bits.\n", MAX_MESSAGE_BITS);
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        if (line[i] != '0' && line[i] != '1') {
            fprintf(stderr, "Invalid bit '%c'. Use only 0 or 1.\n", line[i]);
            return 0;
        }
    }

    i32 *message = malloc(len * sizeof(i32));
    if (!message) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        message[i] = line[i] - '0';
    }

    *message_out = message;
    return len;
}

i32 run_bench(int argc, char **argv) {
    BenchOptions opts;
    i32 rc = parse_bench_options(argc, argv, &opts);
    if (rc == 1) {
        print_usage(argv[0]); // or "knapsack" if you prefer
        return 0;
    }
    if (rc < 0) {
        print_usage(argv[0]);
        return 1;
    }
    u32 seed = opts.has_seed ? opts.seed : (u32)time(NULL);
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);

    if (strcmp(opts.format, "csv") != 0) {
        fprintf(stderr, "Unsupported format: %s\n", opts.format);
        return 1;
    }
    if (opts.message_bits) {
        size_t msg_n = strlen(opts.message_bits);
        if (msg_n == 0) {
            fprintf(stderr, "--msg must not be empty\n");
            return 1;
        }
        if (msg_n > (size_t)INT_MAX) {
            fprintf(stderr, "--msg too long\n");
            return 1;
        }

        i32 n = (i32)msg_n;
        printf("n,keygen_ms,encrypt_ms,decrypt_ms,total_ms,reps,seed\n");
        return bench_run_pipeline_csv(n, n, opts.reps, seed,
                                      opts.message_bits) == 0
                   ? 0
                   : 1;
    }
    printf("n,keygen_ms,encrypt_ms,decrypt_ms,total_ms,reps,seed\n");
    return bench_run_pipeline_csv(opts.n_min, opts.n_max, opts.reps, seed,
                                  opts.message_bits) == 0
               ? 0
               : 1;
}

i32 run_demo(int argc, char **argv) {
    DemoOptions opts;
    i32 rc = parse_demo_options(argc, argv, &opts);
    if (rc == 1) {
        print_usage(argv[0]);
        return 0;
    }
    if (rc < 0) {
        print_usage(argv[0]);
        return 1;
    }

    u32 seed = opts.has_seed ? opts.seed : (u32)time(NULL);
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);

    printf("===knapsack demo===\n");

    i32 *message = NULL;
    size_t n = 0;
    if (opts.message_bits) {
        n = strlen(opts.message_bits);
        if (n == 0 || n > MAX_MESSAGE_BITS) {
            return 1;
        }
        message = malloc(n * sizeof(i32));
        if (!message) {
            return 1;
        }

        if (bits_to_array(opts.message_bits, message, n) != 0) {
            free(message);
            return 1;
        }

    } else {
        n = read_message_bits(&message);
        if (n == 0) {
            return 1;
        }
    }
    KnapsackRunRequest req;
    KnapsackRunOutput out;
    KnapsackRunMetrics metrics;

    req.n = n;
    req.scheme = scheme_mh_get();
    req.message_bits = message;
    req.show_steps = opts.show_steps;
    req.capture_details = 1;

    if (knapsack_run_once(&req, &out, &metrics) != 0) {
        fprintf(stderr, "Roundtrip failed.\n");
        free(message);
        return 1;
    }

    printf("Keys generated: %zu elements\n", out.keypair.n);

    // printf("Private key (superincreasing):\n");
    // for (size_t i = 0; i < out.keypair.n; i++)
    // gmp_printf("w[%zu] = %Zd\n", i, out.keypair);

    // printf("\nPublic key:\n");

    // for (size_t i = 0; i < out.keypair.n; i++)
    // gmp_printf("b[%zu] = %Zd\n", i, out.key.b[i]);

    // gmp_printf("\nm = %Zd\nn = %Zd\n", out.key.m, out.key.n_mult);

    printf("\nPlaintext bits: ");
    for (size_t i = 0; i < n; i++)
        printf("%d", message[i]);
    printf("\n");

    printf("\nCiphertext:\n");
    gmp_printf("C = %Zd\n", out.ciphertext);

    printf("Message decrypted.\n");

    printf("\nDecrypted bits: ");
    for (size_t i = 0; i < out.n; i++)
        printf("%d", out.decrypted_bits[i]);
    printf("\n");

    knapsack_run_output_clear(&out);
    free(message);
    return 0;
}
