#pragma once
#include "common.h"

typedef struct {
    size_t n;            // key length
    mpz_t *w;            // private key sequence (weight)
    mpz_t *b;            // public key sequence (basis)
    mpz_t m;             // modulus (m > sum(weights))
    mpz_t n_mult;        // multiplier (no commond factor with m)
} KnapsackKey;

void keygen_init(KnapsackKey *key, size_t n);
void keygen_generate(KnapsackKey *key);
void keygen_clear(KnapsackKey *key);
