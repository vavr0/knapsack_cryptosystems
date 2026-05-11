#include "scheme_mh_common.h"

KnapStatus mh_key_alloc(MhKey *key, u64 n) {
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

void mh_key_clear(MhKey *key) {
    if (!key) {
        return;
    }
    for (u64 i = 0; i < key->n; i++) {
        mpz_clear(key->priv_weights[i]);
        mpz_clear(key->pub_weights[i]);
    }
    free(key->priv_weights);
    free(key->pub_weights);
    mpz_clears(key->mod, key->mult, key->mult_inv, NULL);

    *key = (MhKey){0};
}

KnapStatus mh_key_build_private(MhKey *key, PrngState *rng) {
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
        mpz_set_ui(delta, 1 + (prng_rand(rng) % MH_DEFAULT_DELTA_MAX));
        mpz_add(delta, delta, sum);
        mpz_set(key->priv_weights[i], delta);
        mpz_add(sum, sum, key->priv_weights[i]);
    }

    // Choose mod > sum(W)
    u64 margin_u64 =
        1 + (prng_rand_u64(rng) % (MH_DEFAULT_MARGIN_FACTOR * key->n));
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
    mpz_clears(delta, sum, margin, NULL);

    return KNAP_OK;
}

void mh_key_build_public(MhKey *key, const u64 *perm) {
    for (u64 i = 0; i < key->n; i++) {
        u64 src = perm ? perm[i] : i;

        mpz_mul(key->pub_weights[i], key->priv_weights[src], key->mult);
        mpz_mod(key->pub_weights[i], key->pub_weights[i], key->mod);
    }
}

void mh_encrypt_impl(const MhKey *key, BitView message, mpz_t ciphertext) {
    mpz_set_ui(ciphertext, 0);
    for (u64 i = 0; i < message.length; i++) {
        if (message.data[i]) {
            mpz_add(ciphertext, ciphertext, key->pub_weights[i]);
        }
    }
}

KnapStatus mh_decrypt_impl(const MhKey *key, const mpz_t ciphertext,
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
