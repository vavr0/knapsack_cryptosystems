#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
#include <gmp.h>
#include <stddef.h>

typedef struct {
    u64 n;
    mpz_t *priv_weights;
    mpz_t *pub_weights;
    u64 *perm;
    u64 *inv_perm;
    mpz_t mod;
    mpz_t mult;
    mpz_t mult_inv;
} MhPermutedKey;

static KnapStatus mh_permuted_key_alloc(MhPermutedKey *key, u64 n) {
    size_t count;

    if (!key || n == 0) {
        return KNAP_ERR_INVALID;
    }

    count = (size_t)n;
    if ((u64)count != n) {
        return KNAP_ERR_ALLOC;
    }
    if (count > SIZE_MAX / sizeof(*key->priv_weights) ||
        count > SIZE_MAX / sizeof(*key->pub_weights) ||
        count > SIZE_MAX / sizeof(*key->perm) ||
        count > SIZE_MAX / sizeof(*key->inv_perm)) {
        return KNAP_ERR_ALLOC;
    }

    key->n = 0;
    key->priv_weights = NULL;
    key->pub_weights = NULL;
    key->perm = NULL;
    key->inv_perm = NULL;

    key->priv_weights = malloc(count * sizeof(*key->priv_weights));
    if (!key->priv_weights) {
        return KNAP_ERR_ALLOC;
    }

    key->pub_weights = malloc(count * sizeof(*key->pub_weights));
    if (!key->pub_weights) {
        free(key->priv_weights);
        key->priv_weights = NULL;
        return KNAP_ERR_ALLOC;
    }

    key->perm = malloc(count * sizeof(*key->perm));
    if (!key->perm) {
        free(key->pub_weights);
        free(key->priv_weights);
        key->pub_weights = NULL;
        key->priv_weights = NULL;
        return KNAP_ERR_ALLOC;
    }

    key->inv_perm = malloc(count * sizeof(*key->inv_perm));
    if (!key->inv_perm) {
        free(key->perm);
        free(key->pub_weights);
        free(key->priv_weights);
        key->inv_perm = NULL;
        key->perm = NULL;
        key->pub_weights = NULL;
        key->priv_weights = NULL;
        return KNAP_ERR_ALLOC;
    }

    key->n = n;
    for (u64 i = 0; i < n; i++) {
        mpz_init(key->priv_weights[i]);
        mpz_init(key->pub_weights[i]);
    }
    mpz_inits(key->mod, key->mult, key->mult_inv, NULL);

    return KNAP_OK;
}

static void mh_permuted_key_clear(MhPermutedKey *key) {
    if (!key) {
        return;
    }

    for (u64 i = 0; i < key->n; i++) {
        mpz_clear(key->priv_weights[i]);
        mpz_clear(key->pub_weights[i]);
    }

    free(key->priv_weights);
    free(key->pub_weights);
    free(key->perm);
    free(key->inv_perm);
    mpz_clears(key->mod, key->mult, key->mult_inv, NULL);

    key->n = 0;
    key->priv_weights = NULL;
    key->pub_weights = NULL;
    key->perm = NULL;
    key->inv_perm = NULL;
}

static void shuffle_u64(u64 *values, u64 n, PrngState *rng) {
    if (!values || !rng || n < 2) {
        return;
    }

    for (u64 i = n - 1; i > 0; i--) {
        u64 j = prng_rand_u64(rng) % (i + 1);
        u64 tmp = values[i];
        values[i] = values[j];
        values[j] = tmp;
    }
}

static KnapStatus mh_permuted_key_build(MhPermutedKey *key, PrngState *rng) {
    mpz_t delta;
    mpz_t sum;
    mpz_t margin;

    if (!key || !rng || key->n == 0) {
        return KNAP_ERR_INVALID;
    }

    mpz_inits(delta, sum, margin, NULL);

    for (u64 i = 0; i < key->n; i++) {
        mpz_set_ui(delta, 1 + (prng_rand(rng) % 16u));
        mpz_add(delta, delta, sum);
        mpz_set(key->priv_weights[i], delta);
        mpz_add(sum, sum, key->priv_weights[i]);
    }

    {
        u64 margin_u64 = 1 + (prng_rand_u64(rng) % (64u * key->n));
        mpz_set_ui(margin, margin_u64);
        mpz_add(key->mod, sum, margin);
    }

    for (;;) {
        mpz_set_ui(key->mult, (unsigned long)prng_rand(rng));
        mpz_mul_2exp(key->mult, key->mult, 32);
        mpz_add_ui(key->mult, key->mult, (unsigned long)prng_rand(rng));
        mpz_mod(key->mult, key->mult, key->mod);

        if (mpz_cmp_ui(key->mult, 2u) < 0) {
            continue;
        }
        if (mpz_invert(key->mult_inv, key->mult, key->mod) != 0) {
            break;
        }
    }

    for (u64 i = 0; i < key->n; i++) {
        key->perm[i] = i;
    }
    shuffle_u64(key->perm, key->n, rng);

    for (u64 i = 0; i < key->n; i++) {
        key->inv_perm[key->perm[i]] = i;
    }

    for (u64 i = 0; i < key->n; i++) {
        u64 src = key->perm[i];
        mpz_mul(key->pub_weights[i], key->priv_weights[src], key->mult);
        mpz_mod(key->pub_weights[i], key->pub_weights[i], key->mod);
    }

    mpz_clears(delta, sum, margin, NULL);
    return KNAP_OK;
}

static void mh_permuted_encrypt_impl(const MhPermutedKey *key, BitView message,
                                     mpz_t ciphertext) {
    mpz_set_ui(ciphertext, 0);

    for (u64 i = 0; i < key->n; i++) {
        if (message.data[key->perm[i]]) {
            mpz_add(ciphertext, ciphertext, key->pub_weights[i]);
        }
    }
}

static KnapStatus mh_permuted_decrypt_impl(const MhPermutedKey *key,
                                           const mpz_t ciphertext,
                                           BitBuf *message) {
    mpz_t s;

    mpz_init(s);
    mpz_mul(s, ciphertext, key->mult_inv);
    mpz_mod(s, s, key->mod);

    for (u64 i = key->n; i-- > 0;) {
        u64 orig_index = key->perm[i];
        u8 bit = 0;

        if (mpz_cmp(s, key->priv_weights[orig_index]) >= 0) {
            bit = 1;
            mpz_sub(s, s, key->priv_weights[orig_index]);
        }
        message->data[orig_index] = bit;
    }

    if (mpz_cmp_ui(s, 0) != 0) {
        mpz_clear(s);
        return KNAP_ERR_CRYPTO;
    }

    mpz_clear(s);
    return KNAP_OK;
}

static MhPermutedKey *
mh_permuted_key_from_scheme_key(const SchemeKey *scheme_key) {
    if (!scheme_key || !scheme_key->data) {
        return NULL;
    }
    return (MhPermutedKey *)scheme_key->data;
}

static KnapStatus mh_permuted_keygen(const SchemeKeygenParams *params,
                                     SchemeKey *out_scheme_key) {
    PrngState rng = {0};
    MhPermutedKey *key;
    KnapStatus status;

    if (!params || !out_scheme_key || params->n == 0) {
        return KNAP_ERR_INVALID;
    }

    prng_seed(&rng, params->initstate, params->initseq);
    out_scheme_key->data = NULL;
    out_scheme_key->n = 0;

    key = malloc(sizeof(*key));
    if (!key) {
        return KNAP_ERR_ALLOC;
    }

    status = mh_permuted_key_alloc(key, params->n);
    if (status != KNAP_OK) {
        free(key);
        return status;
    }

    status = mh_permuted_key_build(key, &rng);
    if (status != KNAP_OK) {
        mh_permuted_key_clear(key);
        free(key);
        return status;
    }

    out_scheme_key->data = key;
    out_scheme_key->n = params->n;
    return KNAP_OK;
}

static KnapStatus mh_permuted_encrypt(const SchemeKey *scheme_key,
                                      BitView message, mpz_t out_ciphertext) {
    MhPermutedKey *key;

    if (!scheme_key || !scheme_key->data || !message.data ||
        message.length == 0) {
        return KNAP_ERR_INVALID;
    }
    if (message.length != scheme_key->n) {
        return KNAP_ERR_INVALID;
    }

    key = mh_permuted_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    mh_permuted_encrypt_impl(key, message, out_ciphertext);
    return KNAP_OK;
}

static KnapStatus mh_permuted_decrypt(const SchemeKey *scheme_key,
                                      const mpz_t ciphertext,
                                      BitBuf *out_message) {
    MhPermutedKey *key;
    KnapStatus status;

    if (!scheme_key || !scheme_key->data || !out_message ||
        scheme_key->n == 0) {
        return KNAP_ERR_INVALID;
    }

    key = mh_permuted_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    status = bit_buf_alloc(out_message, (size_t)scheme_key->n);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);
        return status;
    }

    status = mh_permuted_decrypt_impl(key, ciphertext, out_message);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);
        return status;
    }

    return KNAP_OK;
}

static void mh_permuted_scheme_key_clear(SchemeKey *scheme_key) {
    if (!scheme_key) {
        return;
    }

    if (scheme_key->data) {
        MhPermutedKey *key = (MhPermutedKey *)scheme_key->data;
        mh_permuted_key_clear(key);
        free(key);
    }

    scheme_key->data = NULL;
    scheme_key->n = 0;
}

const SchemeOps *scheme_mh_permuted_get(void) {
    static const SchemeOps ops = {
        .info =
            {
                .id = "mh-permuted",
                .name = "Merkle-Hellman (permuted)",
            },
        .keygen = mh_permuted_keygen,
        .encrypt = mh_permuted_encrypt,
        .decrypt = mh_permuted_decrypt,
        .scheme_key_clear = mh_permuted_scheme_key_clear,
    };

    return &ops;
}
