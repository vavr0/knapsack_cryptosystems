#pragma once

#include "scheme.h"

typedef struct {
    size_t n;
    const i32 *message_bits;
    b8 show_steps;
    b8 capture_details;
    const SchemeOps *scheme;
} KnapsackRunRequest;

typedef struct {
    f64 keygen_ms;
    f64 encrypt_ms;
    f64 decrypt_ms;
    f64 total_ms;
} KnapsackRunMetrics;

typedef struct {
    i32 *decrypted_bits;
    size_t n;
    b8 has_key;
    SchemeKeypair keypair;
    const SchemeOps *scheme;
    b8 has_ciphertext;
    mpz_t ciphertext;
} KnapsackRunOutput;

i32 knapsack_run_once(const KnapsackRunRequest *req, KnapsackRunOutput *out,
                      KnapsackRunMetrics *metrics);

void knapsack_run_output_clear(KnapsackRunOutput *out);
