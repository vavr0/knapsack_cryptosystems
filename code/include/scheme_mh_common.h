
#pragma once

#include "buffer.h"
#include "common.h"
#include "error.h"
#include "rand.h"
#include <gmp.h>

typedef struct {
    u64 n;               // key length
    mpz_t *priv_weights; // private key sequence (weight)
    mpz_t *pub_weights;  // public key sequence (basis)
    mpz_t mod;           // modulus (m > sum(weights))
    mpz_t mult;          // multiplier (no commond factor with m)
    mpz_t mult_inv;      // inverse
} MhKey;

KnapStatus mh_key_alloc(MhKey *key, u64 n);
void mh_key_clear(MhKey *key);

KnapStatus mh_key_build_private(MhKey *key, PrngState *rng);
void mh_key_build_public(MhKey *key, const u64 *perm);

void mh_encrypt_impl(const MhKey *key, BitView message, mpz_t ciphertext);
KnapStatus mh_decrypt_impl(const MhKey *key, const mpz_t ciphertext,
                              BitBuf *message);
