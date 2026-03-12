#pragma once

#include "bitvec.h"
#include "scheme.h"

typedef struct {
    const SchemeOps *scheme;
    SchemeKeygenParams params;
    BitView message;
    b8 show_steps;
    b8 capture_details;
} KnapsackRunRequest;

typedef struct {
    f64 keygen_ms;
    f64 encrypt_ms;
    f64 decrypt_ms;
    f64 total_ms;
} KnapsackRunMetrics;

typedef struct {
    BitBuf decrypted_message;
    b8 has_key;
    SchemeKeypair keypair;
    b8 has_ciphertext;
    mpz_t ciphertext;
} KnapsackRunOutput;

i32 knapsack_run_once(const KnapsackRunRequest *req, KnapsackRunOutput *out,
                      KnapsackRunMetrics *metrics);

void knapsack_run_output_clear(KnapsackRunOutput *out);
