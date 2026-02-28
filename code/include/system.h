#pragma once

#include "scheme.h"

typedef struct {
    size_t n;
    const int *message_bits;
    int show_steps;
    int capture_details;
    const SchemeOps *scheme; 
} KnapsackRunRequest;

typedef struct {
    double keygen_ms;
    double encrypt_ms;
    double decrypt_ms;
    double total_ms;
} KnapsackRunMetrics;

typedef struct {
    int *decrypted_bits;
    size_t n;
    int has_key;
    SchemeKeypair keypair;
    const SchemeOps *scheme;
    int has_ciphertext;
    mpz_t ciphertext;
} KnapsackRunOutput;

int knapsack_run_once(const KnapsackRunRequest *req,
                      KnapsackRunOutput *out,
                      KnapsackRunMetrics *metrics);

void knapsack_run_output_clear(KnapsackRunOutput *out);
