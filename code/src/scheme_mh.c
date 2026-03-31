#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
#include <gmp.h>
#include <stddef.h>
#include <time.h>

typedef struct {
    u64 n;               // key length
    mpz_t *priv_weights; // private key sequence (weight)
    mpz_t *pub_weights;  // public key sequence (basis)
    mpz_t mod;           // modulus (m > sum(weights))
    mpz_t mult;          // multiplier (no commond factor with m)
    mpz_t mult_inv;      // inverse
} MhKey;

static KnapStatus mh_key_alloc(MhKey *key, u64 n) {
    u64 i;
    size_t count;

    if (!key || n == 0) {
        return KNAP_ERR_INVALID;
    }

    if (n > (u64)SIZE_MAX) {
        return KNAP_ERR_ALLOC;
    }
    count = (size_t)n;
    if (count > SIZE_MAX / sizeof(*key->priv_weights) ||
        count > SIZE_MAX / sizeof(*key->pub_weights)) {

        return KNAP_ERR_ALLOC;
    }

    key->n = 0;
    key->priv_weights = NULL;
    key->pub_weights = NULL;
    key->priv_weights = malloc((size_t)n * sizeof(*key->priv_weights));
    if (!key->priv_weights) {
        return KNAP_ERR_ALLOC;
    }
    key->pub_weights = malloc((size_t)n * sizeof(*key->pub_weights));
    if (!key->pub_weights) {
        free(key->priv_weights);
        key->priv_weights = NULL;
        return KNAP_ERR_ALLOC;
    }
    key->n = n;
    for (i = 0; i < n; i++) {
        mpz_init(key->priv_weights[i]);
        mpz_init(key->pub_weights[i]);
    }
    mpz_inits(key->mod, key->mult, key->mult_inv, NULL);
    return KNAP_OK;
}

// TODO assumes fully initialized objects
static void mh_key_clear(MhKey *key) {
    for (u64 i = 0; i < key->n; i++) {
        mpz_clear(key->priv_weights[i]);
        mpz_clear(key->pub_weights[i]);
    }
    free(key->priv_weights);
    free(key->pub_weights);
    mpz_clears(key->mod, key->mult, key->mult_inv, NULL);
}

static KnapStatus mh_key_build(MhKey *key, PrngState *rng) {
    if (!key || !rng || key->n == 0) {
        return KNAP_ERR_INVALID;
    }
    mpz_t delta;
    mpz_t sum;
    mpz_t margin;
    mpz_inits(delta, sum, margin, NULL);

    // Build superinceasing sequencce
    // larger delta -> lower density -> easier for lattice-style attacks
    for (u64 i = 0; i < key->n; i++) {
        mpz_set_ui(delta, 1 + (prng_rand(rng) % 16u));
        mpz_add(delta, delta, sum);
        mpz_set(key->priv_weights[i], delta);
        mpz_add(sum, sum, key->priv_weights[i]);
    }

    // Choose mod > sum(W)
    u64 margin_u64 = 1 + (prng_rand_u64(rng) % (64u * key->n));
    mpz_set_ui(margin, margin_u64);
    mpz_add(key->mod, sum, margin);

    // Choose multiplier coprime to m
    for (;;) {
        mpz_set_ui(key->mult, prng_rand_u64(rng));
        mpz_mod(key->mult, key->mult, key->mod);
        if (mpz_cmp_ui(key->mult, 2u) < 0) {
            continue;
        }
        if (mpz_invert(key->mult_inv, key->mult, key->mod) != 0) {
            break;
        }
    }

    // Generate public key b_i = (w_i * n) mod m
    for (u64 i = 0; i < key->n; i++) {
        mpz_mul(key->pub_weights[i], key->priv_weights[i], key->mult);
        mpz_mod(key->pub_weights[i], key->pub_weights[i], key->mod);
    }
    mpz_clears(delta, sum, margin, NULL);

    return KNAP_OK;
}

static void mh_encrypt_impl(const MhKey *key, BitView message,
                            mpz_t ciphertext) {
    mpz_set_ui(ciphertext, 0);
    for (u64 i = 0; i < message.length; i++) {
        if (message.data[i]) {
            mpz_add(ciphertext, ciphertext, key->pub_weights[i]);
        }
    }
}

static KnapStatus mh_decrypt_impl(const MhKey *key, const mpz_t ciphertext,
                                  BitBuf *message) {
    mpz_t s;
    mpz_init(s);

    // Compute s = (C * n_inv) mod m
    mpz_mul(s, ciphertext, key->mult_inv);
    mpz_mod(s, s, key->mod);

    // Greedy algorithm: recover bits from largest to smallest
    for (u64 i = key->n; i-- > 0;) {

        if (mpz_cmp(s, key->priv_weights[i]) >= 0) {
            message->data[i] = 1;

            mpz_sub(s, s, key->priv_weights[i]);
        } else {
            message->data[i] = 0;
        }
    }

    if (mpz_cmp_ui(s, 0) != 0) {
        mpz_clear(s);
        return KNAP_ERR_CRYPTO;
    }
    mpz_clear(s);

    return KNAP_OK;
}

static MhKey *mh_key_from_scheme_key(const SchemeKey *scheme_key) {
    if (!scheme_key || !scheme_key->data) {
        return NULL;
    }
    return (MhKey *)scheme_key->data;
}

static KnapStatus mh_keygen(const SchemeKeygenParams *params,
                            SchemeKey *out_scheme_key) {
    if (!params || !out_scheme_key || params->n == 0) {
        return KNAP_ERR_INVALID;
    }
    PrngState rng = {0};
    prng_seed(&rng, params->initstate, params->initseq);
    out_scheme_key->data = NULL;
    out_scheme_key->n = 0;

    MhKey *key = (MhKey *)malloc(sizeof(*key));
    if (!key) {
        return KNAP_ERR_ALLOC;
    }
    KnapStatus status = mh_key_alloc(key, params->n);
    if (status != KNAP_OK) {
        free(key);
        return status;
    }

    status = mh_key_build(key, &rng);
    if (status != KNAP_OK) {
        mh_key_clear(key);
        free(key);

        return status;
    }
    out_scheme_key->data = key;
    out_scheme_key->n = params->n;

    return KNAP_OK;
}

static KnapStatus mh_encrypt(const SchemeKey *scheme_key, BitView message,
                             mpz_t out_ciphertext) {
    if (!scheme_key || !scheme_key->data || message.length == 0 ||
        !message.data) {
        return KNAP_ERR_INVALID;
    }

    if (message.length != scheme_key->n) {
        return KNAP_ERR_INVALID;
    }

    MhKey *key = mh_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }
    mh_encrypt_impl(key, message, out_ciphertext);
    return KNAP_OK;
}

static KnapStatus mh_decrypt(const SchemeKey *scheme_key,
                             const mpz_t ciphertext, BitBuf *out_message) {
    KnapStatus status;
    if (!scheme_key || !out_message || !scheme_key->data ||
        scheme_key->n == 0) {
        return KNAP_ERR_INVALID;
    }

    MhKey *key = mh_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    status = bit_buf_alloc(out_message, scheme_key->n);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);

        return status;
    }

    status = mh_decrypt_impl(key, ciphertext, out_message);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);

        return status;
    }
    return KNAP_OK;
}

static void mh_scheme_key_clear(SchemeKey *scheme_key) {
    if (!scheme_key) {
        return;
    }
    if (scheme_key->data) {
        MhKey *key = (MhKey *)scheme_key->data;
        mh_key_clear(key);
        free(key);
    }
    scheme_key->data = NULL;
    scheme_key->n = 0;
}

const SchemeOps *scheme_mh_get(void) {
    static const SchemeOps ops = {
        .info =
            {
                .id = "mh-classic",
                .name = "Merkle-Hellman (classic)",
            },
        .keygen = mh_keygen,
        .encrypt = mh_encrypt,
        .decrypt = mh_decrypt,
        .scheme_key_clear = mh_scheme_key_clear,
    };
    return &ops;
}
