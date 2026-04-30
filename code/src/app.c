#include "app.h"
#include "bench.h"
#include "buffer.h"
#include "cli.h"
#include "common.h"
#include "error.h"
#include "plaintext.h"
#include "scheme.h"
#include "seed.h"
#include <gmp.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_TEXT_BLOCK_SIZE 128u

static KnapStatus read_message(TextBuf *out) {
    char line[512];

    if (!out) {
        return KNAP_ERR_INVALID;
    }

    printf("Enter plaintext: ");
    if (!fgets(line, sizeof(line), stdin)) {
        return KNAP_ERR_INVALID;
    }

    size_t len = strcspn(line, "\n");
    line[len] = '\0';

    if (len == 0) {
        fprintf(stderr, "Invalid length. Use non-empty plaintext.\n");
        return KNAP_ERR_INVALID;
    }

    return text_buf_from_cstr(out, line);
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
    fprintf(stderr, "\n");
    fprintf(stderr, "Demo options:\n");
    fprintf(stderr, "  --bits BITS        Raw bitstring input, e.g. 101010\n");
    fprintf(stderr, "  --msg TEXT         Plaintext input; converted to bits\n");
    fprintf(stderr, "  --n BITS           Demo block size in bits; text default is 128\n");
    fprintf(stderr, "  --scheme ID        Scheme: mh-classic, mh, mh-permuted\n");
    fprintf(stderr, "  --seed SEED        Deterministic seed\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Bench options:\n");
    fprintf(stderr, "  --bits BITS        Raw bitstring input\n");
    fprintf(stderr, "  --n BITS           Random message length in bits\n");
    fprintf(stderr, "  --reps N           Measurement repetitions; default is 10\n");
    fprintf(stderr, "  --format csv       CSV output\n");
    fprintf(stderr, "  --scheme ID        Scheme: mh-classic, mh, mh-permuted\n");
    fprintf(stderr, "  --seed SEED        Deterministic seed\n");
}

static KnapStatus demo_run(CliFlags *flags) {
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKey scheme_key = {0};
    SchemeKeygenParams params = {0};
    BitBuf decrypted = {0};
    mpz_t ciphertext;
    u64 seed_pair[2];

    BitBlocks blocks = {0};
    u64 block_size;

    status = seed_resolve_pair(flags->has_seed, flags->seed, seed_pair);
    if (status != KNAP_OK) {
        return status;
    }

    printf("===knapsack demo===\n");

    if (flags->input_mode == CLI_INPUT_NONE) {
        status = read_message(&flags->text_message);
        if (status != KNAP_OK) {
            return status;
        }
        flags->input_mode = CLI_INPUT_TEXT;
    }

    if (flags->input_mode == CLI_INPUT_TEXT) {
        status = bit_buf_from_text(&flags->bits_message, &flags->text_message);
        if (status != KNAP_OK) {

            return status;
        }
    }

    if (flags->n != 0) {
        block_size = flags->n;
    } else if (flags->input_mode == CLI_INPUT_TEXT) {
        block_size = DEFAULT_TEXT_BLOCK_SIZE;
    } else {
        block_size = flags->bits_message.length;
    }

    status = blocks_from_bits(&blocks, bit_buf_view(&flags->bits_message),
                              block_size);
    if (status != KNAP_OK) {
        bit_blocks_clear(&blocks);
        return status;
    }

    scheme = scheme_resolve(flags->scheme_id);
    if (!scheme) {
        bit_blocks_clear(&blocks);
        return KNAP_ERR_INVALID;
    }

    params.n = blocks.block_size;
    params.initstate = seed_pair[0];
    params.initseq = seed_pair[1];
    params.flags = 0;
    mpz_init(ciphertext);

    status = scheme->keygen(&params, &scheme_key);
    if (status != KNAP_OK) {
        mpz_clear(ciphertext);
        bit_blocks_clear(&blocks);

        return status;
    }

    status = bit_buf_alloc(&decrypted, (size_t)blocks.bits.length);
    if (status != KNAP_OK) {
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);
        bit_blocks_clear(&blocks);
        return status;
    }

    for (u64 i = 0; i < blocks.block_count; i++) {
        BitBuf decrypted_block = {0};
        BitView block = get_bit_block(&blocks, i);

        status = scheme->encrypt(&scheme_key, block, ciphertext);
        if (status != KNAP_OK) {
            bit_buf_clear(&decrypted_block);
            bit_buf_clear(&decrypted);
            bit_blocks_clear(&blocks);
            scheme->scheme_key_clear(&scheme_key);
            mpz_clear(ciphertext);

            return status;
        }

        status = scheme->decrypt(&scheme_key, ciphertext, &decrypted_block);
        if (status != KNAP_OK) {
            bit_buf_clear(&decrypted_block);
            bit_buf_clear(&decrypted);
            bit_blocks_clear(&blocks);
            scheme->scheme_key_clear(&scheme_key);
            mpz_clear(ciphertext);

            return status;
        }
        memcpy(decrypted.data + (i * blocks.block_size), decrypted_block.data,
               (size_t)blocks.block_size);

        bit_buf_clear(&decrypted_block);
    }

    if (!bit_buf_equal(&decrypted, &blocks.bits)) {
        bit_buf_clear(&decrypted);
        bit_blocks_clear(&blocks);
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);

        return KNAP_ERR_CRYPTO;
    }

    status = print_demo_result(&flags->bits_message, ciphertext, &decrypted);
    if (status != KNAP_OK) {
        bit_blocks_clear(&blocks);
        bit_buf_clear(&decrypted);
        scheme->scheme_key_clear(&scheme_key);
        mpz_clear(ciphertext);

        return status;
    }

    bit_blocks_clear(&blocks);
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
