#include "app.h"
#include "bench.h"
#include "cli.h"
#include "system.h"
#include <string.h>
#include <time.h>

#define MAX_MESSAGE_BITS 128

void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s demo  [options]\n", prog);
    fprintf(stderr, "  %s bench [options]\n", prog);
}

static size_t read_message_bits(int **message_out) {
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

    int *message = malloc(len * sizeof(int));
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

int run_bench(int argc, char **argv) {
    BenchOptions opts;
    int rc = parse_bench_options(argc, argv, &opts);
    if (rc == 1) {
        print_usage(argv[0]);   // or "knapsack" if you prefer
        return 0;
    }
    if (rc < 0) {
        print_usage(argv[0]);
        return 1;
    }
    unsigned int seed = opts.has_seed ? opts.seed : (unsigned int)time(NULL);
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);
    if (strcmp(opts.format, "csv") != 0) {
        fprintf(stderr, "Unsupported format: %s\n", opts.format);
        return 1;
    }
    int ok;
    if (strcmp(opts.kind, "pipeline") == 0) {
        printf("n,keygen_ms,encrypt_ms,decrypt_ms,total_ms,reps,seed\n");
        ok = bench_run_pipeline_csv(opts.n_min, opts.n_max, opts.reps, seed);
    } else if (strcmp(opts.kind, "compare") == 0) {
        printf("n,brute_ms,super_ms,reps,seed\n");
        ok = bench_run_compare_csv(opts.n_min, opts.n_max, opts.reps, seed);
    } else {
        fprintf(stderr, "Unsupported bench kind: %s\n", opts.kind);
        return 1;
    }
    return ok == 0 ? 0 : 1;
}

int run_demo(int argc, char **argv) {
    DemoOptions opts;
    int rc = parse_demo_options(argc, argv, &opts);
    if (rc == 1) {
        print_usage(argv[0]);
        return 0;
    }
    if (rc < 0) {
        print_usage(argv[0]);
        return 1;
    }
    unsigned int seed = opts.has_seed ? opts.seed : (unsigned int)time(NULL);
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);

    printf("===knapsack demo===\n");

    int *message = NULL;
    size_t n = 0;
    if (opts.message_bits) {
        n = strlen(opts.message_bits);
        if (n == 0 || n > MAX_MESSAGE_BITS)
            return 1;
        message = malloc(n * sizeof(int));
        if (!message)
            return 1;
        for (size_t i = 0; i < n; i++)
            message[i] = opts.message_bits[i] - '0';
    } else {
        n = read_message_bits(&message);
        if (n == 0)
            return 1;
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
    //for (size_t i = 0; i < out.keypair.n; i++)
        //gmp_printf("w[%zu] = %Zd\n", i, out.keypair);

    // printf("\nPublic key:\n");

    //for (size_t i = 0; i < out.keypair.n; i++)
        //gmp_printf("b[%zu] = %Zd\n", i, out.key.b[i]);

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
