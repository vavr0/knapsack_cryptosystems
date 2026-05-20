#pragma once

#include "buffer.h"
#include "common.h"
#include "error.h"
#include "rand.h"
#include <gmp.h>

// Default upper bound for additive superincreasing increment delta.
#define MH_DEFAULT_DELTA_MAX 64u

// Default factor for modulus margin bound: margin <= factor * n.
#define MH_DEFAULT_MARGIN_FACTOR 64u

// Classic Merkle-Hellman 
// key pair/trapdoor.
typedef struct {
    u64 n;
    mpz_t *priv_weights;
    mpz_t *pub_weights;
    mpz_t mod;
    mpz_t mult;
    mpz_t mult_inv;
} MhKey;

KnapStatus mh_key_alloc(MhKey *key, u64 n);
void mh_key_clear(MhKey *key);

KnapStatus mh_key_build_private(MhKey *key, PrngState *rng);
void mh_key_build_public(MhKey *key, const u64 *perm);

void mh_encrypt_impl(const MhKey *key, BitView message, mpz_t ciphertext);
KnapStatus mh_decrypt_impl(const MhKey *key, const mpz_t ciphertext,
                           BitBuf *message);

KnapStatus mh_choose_multiplier(mpz_t mult, mpz_t mult_inv, const mpz_t mod,
                                PrngState *rng);
