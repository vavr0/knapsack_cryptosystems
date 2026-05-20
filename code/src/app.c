#include "app.h"
#include "attack_mh.h"
#include "bench.h"
#include "buffer.h"
#include "cli.h"
#include "common.h"
#include "error.h"
#include "output.h"
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

// Read one non-empty plaintext line 
// from stdin.
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

// Allocate an array of initialized GMP ciphertext 
// integers.
static KnapStatus ciphertexts_alloc(mpz_t **out, u64 count) {
    mpz_t *ciphertexts;

    if (!out || count == 0) {
        return KNAP_ERR_INVALID;
    }
    if (count > (u64)SIZE_MAX / sizeof(*ciphertexts)) {
        return KNAP_ERR_ALLOC;
    }

    ciphertexts = malloc((size_t)count * sizeof(*ciphertexts));
    if (!ciphertexts) {
        return KNAP_ERR_ALLOC;
    }

    for (u64 i = 0; i < count; i++) {
        mpz_init(ciphertexts[i]);
    }

    *out = ciphertexts;
    return KNAP_OK;
}

static void ciphertexts_clear(mpz_t *ciphertexts, u64 count) {
    if (!ciphertexts) {
        return;
    }

    for (u64 i = 0; i < count; i++) {
        mpz_clear(ciphertexts[i]);
    }
    free(ciphertexts);
}

// Run demo flow: prepare input blocks, 
// encrypt, decrypt, and print result.
static KnapStatus demo_run(CliFlags *flags) {
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKey scheme_key = {0};
    SchemeKeygenParams params = {0};
    BitBuf decrypted = {0};
    mpz_t *ciphertexts;
    u64 seed[2];

    BitBlocks blocks = {0};
    u64 block_size;

    status = seed_resolve_pair(flags->has_seed, flags->seed, seed);
    if (status != KNAP_OK) {
        return status;
    }

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

    status = ciphertexts_alloc(&ciphertexts, blocks.block_count);
    if (status != KNAP_OK) {
        bit_blocks_clear(&blocks);
        return status;
    }

    params.n = blocks.block_size;
    params.initstate = seed[0];
    params.initseq = seed[1];
    params.flags = 0;

    status = scheme->keygen(&params, &scheme_key);
    if (status != KNAP_OK) {
        ciphertexts_clear(ciphertexts, blocks.block_count);
        bit_blocks_clear(&blocks);

        return status;
    }

    status = bit_buf_alloc(&decrypted, (size_t)blocks.bits.length);
    if (status != KNAP_OK) {
        scheme->scheme_key_clear(&scheme_key);
        ciphertexts_clear(ciphertexts, blocks.block_count);
        bit_blocks_clear(&blocks);
        return status;
    }

    for (u64 i = 0; i < blocks.block_count; i++) {
        BitBuf decrypted_block = {0};
        BitView block = get_bit_block(&blocks, i);

        status = scheme->encrypt(&scheme_key, block, ciphertexts[i]);
        if (status != KNAP_OK) {
            bit_buf_clear(&decrypted_block);
            bit_buf_clear(&decrypted);
            ciphertexts_clear(ciphertexts, blocks.block_count);
            bit_blocks_clear(&blocks);
            scheme->scheme_key_clear(&scheme_key);

            return status;
        }

        status = scheme->decrypt(&scheme_key, ciphertexts[i], &decrypted_block);
        if (status != KNAP_OK) {
            bit_buf_clear(&decrypted_block);
            bit_buf_clear(&decrypted);
            ciphertexts_clear(ciphertexts, blocks.block_count);
            bit_blocks_clear(&blocks);
            scheme->scheme_key_clear(&scheme_key);

            return status;
        }
        memcpy(decrypted.data + (i * blocks.block_size), decrypted_block.data,
               (size_t)blocks.block_size);

        bit_buf_clear(&decrypted_block);
    }

    if (!bit_buf_equal(&decrypted, &blocks.bits)) {
        bit_buf_clear(&decrypted);
        ciphertexts_clear(ciphertexts, blocks.block_count);
        bit_blocks_clear(&blocks);
        scheme->scheme_key_clear(&scheme_key);

        return KNAP_ERR_CRYPTO;
    }

    status = print_demo_result(flags, scheme, &blocks, ciphertexts, &decrypted);

    if (status != KNAP_OK) {
        ciphertexts_clear(ciphertexts, blocks.block_count);
        bit_blocks_clear(&blocks);
        bit_buf_clear(&decrypted);
        scheme->scheme_key_clear(&scheme_key);

        return status;
    }

    ciphertexts_clear(ciphertexts, blocks.block_count);
    bit_blocks_clear(&blocks);
    bit_buf_clear(&decrypted);
    scheme->scheme_key_clear(&scheme_key);

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
    } else if (flags.mode == CLI_MODE_BENCH) {
        status = bench_run(&flags);
    } else {
        status = attack_mh_run(&flags);
    }

    cli_flags_clear(&flags);

    return status;
}
