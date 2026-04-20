#pragma once

#include "common.h"
#include "error.h"
#include "scheme.h"
#include <gmp.h>

typedef struct {
    const SchemeOps *scheme;
    const char *plaintext;
    u64 block_bits;
    u64 initstate;
    u64 initseq;
} DemoPlaintextRequest;

typedef struct {
    char *plaintext;
    u64 plaintext_len;
    u64 block_bits;
    u64 block_count;
    mpz_t *cipher_blocks;
    char *decrypted_text;
} DemoPlaintextResult;

KnapStatus demo_plaintext_validate_request(const DemoPlaintextRequest *req);
KnapStatus demo_plaintext_run(const DemoPlaintextRequest *req,
                              DemoPlaintextResult *out);
KnapStatus demo_plaintext_print(const DemoPlaintextResult *result);
void demo_plaintext_result_clear(DemoPlaintextResult *result);
