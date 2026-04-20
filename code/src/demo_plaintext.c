#include "demo_plaintext.h"
#include "bitvec.h"
#include "common.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

static KnapStatus demo_plaintext_copy_cstr(const char *src, char **out) {
    size_t len;
    char *copy;

    if (!src || !out) {
        return KNAP_ERR_INVALID;
    }

    len = strlen(src);
    if (len == 0) {
        return KNAP_ERR_INVALID;
    }
    if (len == SIZE_MAX) {
        return KNAP_ERR_ALLOC;
    }

    copy = malloc(len + 1);
    if (!copy) {
        return KNAP_ERR_ALLOC;
    }

    memcpy(copy, src, len + 1);
    *out = copy;
    return KNAP_OK;
}

static void demo_plaintext_bytes_to_bits(const u8 *bytes, size_t byte_len,
                                         u8 *out_bits) {
    if (!bytes || !out_bits) {
        return;
    }

    for (size_t i = 0; i < byte_len; i++) {
        u8 byte = bytes[i];

        for (size_t bit = 0; bit < 8; bit++) {
            out_bits[i * 8 + bit] = (u8)((byte >> (7u - bit)) & 1u);
        }
    }
}

static KnapStatus demo_plaintext_bits_to_cstr(BitView bits, size_t byte_len,
                                              char **out_text) {
    char *text;

    if (!out_text || !bit_view_is_valid(bits)) {
        return KNAP_ERR_INVALID;
    }
    if (byte_len == 0 || byte_len == SIZE_MAX) {
        return KNAP_ERR_INVALID;
    }
    if (bits.length != (u64)(byte_len * 8)) {
        return KNAP_ERR_INVALID;
    }

    text = malloc(byte_len + 1);
    if (!text) {
        return KNAP_ERR_ALLOC;
    }

    for (size_t i = 0; i < byte_len; i++) {
        u8 byte = 0;

        for (size_t bit = 0; bit < 8; bit++) {
            byte = (u8)(byte << 1);
            byte = (u8)(byte | bits.data[i * 8 + bit]);
        }
        text[i] = (char)byte;
    }

    text[byte_len] = '\0';
    *out_text = text;
    return KNAP_OK;
}

static void demo_plaintext_cipher_blocks_clear(mpz_t *cipher_blocks,
                                               size_t block_count) {
    if (!cipher_blocks) {
        return;
    }

    for (size_t i = 0; i < block_count; i++) {
        mpz_clear(cipher_blocks[i]);
    }
    free(cipher_blocks);
}

static KnapStatus demo_plaintext_cipher_blocks_alloc(mpz_t **out_blocks,
                                                     size_t block_count) {
    mpz_t *blocks;

    if (!out_blocks || block_count == 0) {
        return KNAP_ERR_INVALID;
    }
    if (block_count > SIZE_MAX / sizeof(*blocks)) {
        return KNAP_ERR_ALLOC;
    }

    blocks = malloc(block_count * sizeof(*blocks));
    if (!blocks) {
        return KNAP_ERR_ALLOC;
    }

    for (size_t i = 0; i < block_count; i++) {
        mpz_init(blocks[i]);
    }

    *out_blocks = blocks;
    return KNAP_OK;
}

KnapStatus demo_plaintext_validate_request(const DemoPlaintextRequest *req) {
    if (!req || !req->scheme || !req->plaintext) {
        return KNAP_ERR_INVALID;
    }
    if (req->plaintext[0] == '\0') {
        return KNAP_ERR_INVALID;
    }
    if (req->block_bits == 0 || (req->block_bits % 8u) != 0) {
        return KNAP_ERR_INVALID;
    }
    if (req->block_bits > (u64)SIZE_MAX) {
        return KNAP_ERR_ALLOC;
    }
    if (!req->scheme->keygen || !req->scheme->encrypt || !req->scheme->decrypt ||
        !req->scheme->scheme_key_clear) {
        return KNAP_ERR_INVALID;
    }

    return KNAP_OK;
}

KnapStatus demo_plaintext_run(const DemoPlaintextRequest *req,
                              DemoPlaintextResult *out) {
    KnapStatus status;
    SchemeKeygenParams params = {0};
    SchemeKey scheme_key = {0};
    DemoPlaintextResult result = {0};
    BitBuf plaintext_bits = {0};
    BitBuf decrypted_bits = {0};
    size_t plaintext_len;
    size_t block_bits;
    size_t raw_bit_len;
    size_t block_count;
    size_t padded_bit_len;

    if (!out) {
        return KNAP_ERR_INVALID;
    }

    status = demo_plaintext_validate_request(req);
    if (status != KNAP_OK) {
        return status;
    }

    plaintext_len = strlen(req->plaintext);
    block_bits = (size_t)req->block_bits;
    if (plaintext_len > SIZE_MAX / 8) {
        return KNAP_ERR_ALLOC;
    }

    raw_bit_len = plaintext_len * 8;
    block_count = raw_bit_len / block_bits;
    if ((raw_bit_len % block_bits) != 0) {
        block_count++;
    }
    if (block_count == 0 || block_count > SIZE_MAX / block_bits) {
        return KNAP_ERR_ALLOC;
    }

    padded_bit_len = block_count * block_bits;

    status = bit_buf_alloc(&plaintext_bits, padded_bit_len);
    if (status != KNAP_OK) {
        return status;
    }

    demo_plaintext_bytes_to_bits((const u8 *)req->plaintext, plaintext_len,
                                 plaintext_bits.data);

    status = bit_buf_alloc(&decrypted_bits, padded_bit_len);
    if (status != KNAP_OK) {
        bit_buf_clear(&plaintext_bits);
        return status;
    }

    status = demo_plaintext_cipher_blocks_alloc(&result.cipher_blocks, block_count);
    if (status != KNAP_OK) {
        bit_buf_clear(&plaintext_bits);
        bit_buf_clear(&decrypted_bits);
        return status;
    }

    params.n = req->block_bits;
    params.initstate = req->initstate;
    params.initseq = req->initseq;
    params.flags = 0;

    status = req->scheme->keygen(&params, &scheme_key);
    if (status != KNAP_OK) {
        demo_plaintext_cipher_blocks_clear(result.cipher_blocks, block_count);
        bit_buf_clear(&plaintext_bits);
        bit_buf_clear(&decrypted_bits);
        return status;
    }

    for (size_t i = 0; i < block_count; i++) {
        BitView block = {
            .data = plaintext_bits.data + (i * block_bits),
            .length = req->block_bits,
        };
        BitBuf decrypted_block = {0};

        status = req->scheme->encrypt(&scheme_key, block, result.cipher_blocks[i]);
        if (status != KNAP_OK) {
            bit_buf_clear(&decrypted_block);
            req->scheme->scheme_key_clear(&scheme_key);
            demo_plaintext_cipher_blocks_clear(result.cipher_blocks, block_count);
            bit_buf_clear(&plaintext_bits);
            bit_buf_clear(&decrypted_bits);
            return status;
        }

        status = req->scheme->decrypt(&scheme_key, result.cipher_blocks[i],
                                      &decrypted_block);
        if (status != KNAP_OK) {
            bit_buf_clear(&decrypted_block);
            req->scheme->scheme_key_clear(&scheme_key);
            demo_plaintext_cipher_blocks_clear(result.cipher_blocks, block_count);
            bit_buf_clear(&plaintext_bits);
            bit_buf_clear(&decrypted_bits);
            return status;
        }

        if (decrypted_block.length != req->block_bits) {
            bit_buf_clear(&decrypted_block);
            req->scheme->scheme_key_clear(&scheme_key);
            demo_plaintext_cipher_blocks_clear(result.cipher_blocks, block_count);
            bit_buf_clear(&plaintext_bits);
            bit_buf_clear(&decrypted_bits);
            return KNAP_ERR_CRYPTO;
        }

        memcpy(decrypted_bits.data + (i * block_bits), decrypted_block.data,
               block_bits);
        bit_buf_clear(&decrypted_block);
    }

    req->scheme->scheme_key_clear(&scheme_key);

    if (!bit_buf_equal(&plaintext_bits, &decrypted_bits)) {
        demo_plaintext_cipher_blocks_clear(result.cipher_blocks, block_count);
        bit_buf_clear(&plaintext_bits);
        bit_buf_clear(&decrypted_bits);
        return KNAP_ERR_CRYPTO;
    }

    status = demo_plaintext_copy_cstr(req->plaintext, &result.plaintext);
    if (status != KNAP_OK) {
        demo_plaintext_cipher_blocks_clear(result.cipher_blocks, block_count);
        bit_buf_clear(&plaintext_bits);
        bit_buf_clear(&decrypted_bits);
        return status;
    }

    status = demo_plaintext_bits_to_cstr(
        (BitView){.data = decrypted_bits.data, .length = (u64)raw_bit_len},
        plaintext_len, &result.decrypted_text);
    if (status != KNAP_OK) {
        demo_plaintext_result_clear(&result);
        bit_buf_clear(&plaintext_bits);
        bit_buf_clear(&decrypted_bits);
        return status;
    }

    if (strcmp(result.plaintext, result.decrypted_text) != 0) {
        demo_plaintext_result_clear(&result);
        bit_buf_clear(&plaintext_bits);
        bit_buf_clear(&decrypted_bits);
        return KNAP_ERR_CRYPTO;
    }

    result.plaintext_len = (u64)plaintext_len;
    result.block_bits = req->block_bits;
    result.block_count = (u64)block_count;

    bit_buf_clear(&plaintext_bits);
    bit_buf_clear(&decrypted_bits);
    *out = result;
    return KNAP_OK;
}

KnapStatus demo_plaintext_print(const DemoPlaintextResult *result) {
    if (!result || !result->plaintext || !result->decrypted_text ||
        !result->cipher_blocks || result->block_count == 0) {
        return KNAP_ERR_INVALID;
    }

    printf("Plaintext:  %s\n", result->plaintext);
    for (u64 i = 0; i < result->block_count; i++) {
        gmp_printf("Ciphertext[%llu]: %Zd\n", (unsigned long long)i,
                   result->cipher_blocks[i]);
    }
    printf("Decrypted:  %s\n", result->decrypted_text);
    return KNAP_OK;
}

void demo_plaintext_result_clear(DemoPlaintextResult *result) {
    if (!result) {
        return;
    }

    free(result->plaintext);
    free(result->decrypted_text);
    demo_plaintext_cipher_blocks_clear(result->cipher_blocks,
                                       (size_t)result->block_count);
    *result = (DemoPlaintextResult){0};
}
