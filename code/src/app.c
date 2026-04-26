#include "app.h"
#include "bench.h"
#include "buffer.h"
#include "cli.h"
#include "common.h"
#include "error.h"
#include "scheme.h"
#include "seed.h"
#include <gmp.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static KnapStatus read_bits_message(BitBuf *message_out) {
    char line[256];
    if (!message_out) {
        return KNAP_ERR_INVALID;
    }

    printf("Enter plaintext bits (e.g. 101010): ");
    if (!fgets(line, sizeof(line), stdin)) {
        return KNAP_ERR_INVALID;
    }

    size_t len = strcspn(line, "\n");
    line[len] = '\0';
    if (len == 0) {
        fprintf(stderr, "Invalid length. Use 1... bits.\n");
        return KNAP_ERR_INVALID;
    }

    for (u64 i = 0; i < len; i++) {
        if (line[i] != '0' && line[i] != '1') {
            fprintf(stderr, "Invalid bit '%c'. Use only 0 or 1.\n", line[i]);
            return KNAP_ERR_INVALID;
        }
    }

    u8 *bits = malloc(len * sizeof(*bits));
    if (!bits) {
        fprintf(stderr, "Memory allocation failed.\n");
        return KNAP_ERR_ALLOC;
    }

    for (u64 i = 0; i < len; i++) {
        bits[i] = (u8)(line[i] - '0');
    }
    bit_buf_clear(message_out);
    message_out->data = bits;
    message_out->length = (u64)len;

    return KNAP_OK;
}

static KnapStatus print_demo_result(const BitBuf *bits_message,
                                    const mpz_t ciphertext,
                                    const BitBuf *decrypted_bits) {
    KnapStatus status;
    char *message_str = NULL;
    char *decrypted_str = NULL;

    if (!bits_message || !decrypted_bits) {
        return KNAP_ERR_INVALID;
    }

    status = bit_buf_to_cstr(bits_message, &message_str);
    if (status != KNAP_OK) {
        return status;
    }

    status = bit_buf_to_cstr(decrypted_bits, &decrypted_str);
    if (status != KNAP_OK) {
        free(message_str);
        return status;
    }

    printf("Plaintext:  %s\n", message_str);
    printf("Ciphertext: ");
    gmp_printf("C = %Zd\n", ciphertext);
    printf("Decrypted:  %s\n", decrypted_str);

    free(message_str);
    free(decrypted_str);
    return KNAP_OK;
}

void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s demo  [options]\n", prog);
    fprintf(stderr, "  %s bench [options]\n", prog);
}

static KnapStatus demo_run(CliFlags *flags) {
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKey scheme_key = {0};
    SchemeKeygenParams params = {0};
    BitBuf decrypted = {0};
    mpz_t ciphertext;
    u64 seed_pair[2];

    status = seed_resolve_pair(flags->has_seed, flags->seed, seed_pair);
    if (status != KNAP_OK) {
        return status;
    }

    printf("===knapsack demo===\n");

    if (flags->bits_message.length == 0) {
        status = read_bits_message(&flags->bits_message);
        if (status != KNAP_OK) {

            return status;
        }
    }
    scheme = scheme_resolve(flags->scheme_id);
    if (!scheme) {
        return KNAP_ERR_INVALID;
    }
    params.n = flags->bits_message.length;
    params.initstate = seed_pair[0];
    params.initseq = seed_pair[1];
    params.flags = 0;
    mpz_init(ciphertext);

    status = scheme->keygen(&params, &scheme_key);
    if (status != KNAP_OK) {
        mpz_clear(ciphertext);

        return status;
    }

    status = scheme->encrypt(&scheme_key, bit_buf_view(&flags->bits_message),
                             ciphertext);
    if (status != KNAP_OK) {
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);

        return status;
    }

    status = scheme->decrypt(&scheme_key, ciphertext, &decrypted);
    if (status != KNAP_OK) {
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);

        return status;
    }

    if (!bit_buf_equal(&decrypted, &flags->bits_message)) {
        bit_buf_clear(&decrypted);
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);

        return KNAP_ERR_CRYPTO;
    }

    status = print_demo_result(&flags->bits_message, ciphertext, &decrypted);
    if (status != KNAP_OK) {
        bit_buf_clear(&decrypted);
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);

        return status;
    }

    bit_buf_clear(&decrypted);
    scheme->scheme_key_clear(&scheme_key);
    mpz_clear(ciphertext);

    return status;
}

KnapStatus app_run(int argc, char **argv) {
    CliFlags flags = {0};
    KnapStatus status = parse_args(argc, argv, &flags);

    if (status == KNAP_STATUS_HELP) {
        print_usage(argv[0]);

        return KNAP_OK;
    }
    if (status != KNAP_OK) {

        return status;
    }

    if (flags.mode == CLI_MODE_DEMO) {
        status = demo_run(&flags);
    } else {
        status = bench_run(&flags);
    }

    cli_flags_clear(&flags);

    return status;
}
