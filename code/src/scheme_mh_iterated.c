#include "buffer.h"
#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
#include "scheme_mh_common.h"
#include <gmp.h>
#include <stddef.h>
#include <stdlib.h>

#define MH_ITERATED_DEFAULT_LAYERS 2u

typedef struct {
    mpz_t mod;
    mpz_t mult;
    mpz_t mult_inv;
} MhIteratedLayer;

typedef struct {
    MhKey base;
    u64 layer_count;
    MhIteratedLayer *extra_layers;
} MhIteratedKey;

static KnapStatus mh_iterated_key_alloc(MhIteratedKey *key, u64 n,
                                        u64 layer_count) {
    KnapStatus status;
    u64 extra_layer_count;

    if (!key || n == 0 || layer_count == 0) {
        return KNAP_ERR_INVALID;
    }

    extra_layer_count = layer_count - 1u;
    if (extra_layer_count > (u64)SIZE_MAX ||
        (size_t)extra_layer_count > SIZE_MAX / sizeof(*key->extra_layers)) {
        return KNAP_ERR_ALLOC;
    }

    *key = (MhIteratedKey){0};

    status = mh_key_alloc(&key->base, n);
    if (status != KNAP_OK) {
        return status;
    }

    key->layer_count = extra_layer_count;
    if (extra_layer_count == 0) {
        return KNAP_OK;
    }

    key->extra_layers =
        malloc((size_t)extra_layer_count * sizeof(*key->extra_layers));
    if (!key->extra_layers) {
        mh_key_clear(&key->base);
        *key = (MhIteratedKey){0};
        return KNAP_ERR_ALLOC;
    }

    for (u64 i = 0; i < extra_layer_count; i++) {
        mpz_inits(key->extra_layers[i].mod, key->extra_layers[i].mult,
                  key->extra_layers[i].mult_inv, NULL);
    }

    return KNAP_OK;
}

static void mh_iterated_key_clear(MhIteratedKey *key) {
    if (!key) {
        return;
    }

    for (u64 i = 0; i < key->layer_count; i++) {
        mpz_clears(key->extra_layers[i].mod, key->extra_layers[i].mult,
                   key->extra_layers[i].mult_inv, NULL);
    }
    free(key->extra_layers);
    mh_key_clear(&key->base);

    *key = (MhIteratedKey){0};
}

static KnapStatus mh_iterated_layer_build(MhIteratedLayer *layer,
                                          const mpz_t *weights, u64 n,
                                          PrngState *rng) {
    mpz_t sum;
    mpz_t margin;
    KnapStatus status;

    if (!layer || !weights || n == 0 || !rng) {
        return KNAP_ERR_INVALID;
    }

    mpz_inits(sum, margin, NULL);

    for (u64 i = 0; i < n; i++) {
        mpz_add(sum, sum, weights[i]);
    }

    u64 margin_u64 = 1 + (prng_rand_u64(rng) % (MH_DEFAULT_MARGIN_FACTOR * n));
    mpz_set_ui(margin, margin_u64);
    mpz_add(layer->mod, sum, margin);

    status =
        mh_choose_multiplier(layer->mult, layer->mult_inv, layer->mod, rng);
    if (status != KNAP_OK) {
        mpz_clears(sum, margin, NULL);

        return status;
    }
    mpz_clears(sum, margin, NULL);

    return KNAP_OK;
}

static void mh_iterated_layer_apply(const MhIteratedLayer *layer,
                                    mpz_t *weights, u64 n) {
    for (u64 i = 0; i < n; i++) {
        mpz_mul(weights[i], weights[i], layer->mult);
        mpz_mod(weights[i], weights[i], layer->mod);
    }
}

static KnapStatus mh_iterated_key_build(MhIteratedKey *key, PrngState *rng) {
    KnapStatus status;

    if (!key || !rng || key->base.n == 0) {
        return KNAP_ERR_INVALID;
    }

    status = mh_key_build_private(&key->base, rng);
    if (status != KNAP_OK) {
        return status;
    }

    mh_key_build_public(&key->base, NULL);

    for (u64 i = 0; i < key->layer_count; i++) {
        status = mh_iterated_layer_build(
            &key->extra_layers[i], key->base.pub_weights, key->base.n, rng);
        if (status != KNAP_OK) {
            return status;
        }
        mh_iterated_layer_apply(&key->extra_layers[i], key->base.pub_weights,
                                key->base.n);
    }

    return KNAP_OK;
}

static MhIteratedKey *
mh_iterated_key_from_scheme_key(const SchemeKey *scheme_key) {
    if (!scheme_key || !scheme_key->data) {
        return NULL;
    }
    return (MhIteratedKey *)scheme_key->data;
}

static KnapStatus mh_iterated_keygen(const SchemeKeygenParams *params,
                                     SchemeKey *out_scheme_key) {
    PrngState rng = {0};
    MhIteratedKey *key;
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

    status = mh_iterated_key_alloc(key, params->n, MH_ITERATED_DEFAULT_LAYERS);
    if (status != KNAP_OK) {
        free(key);
        return status;
    }

    status = mh_iterated_key_build(key, &rng);
    if (status != KNAP_OK) {
        mh_iterated_key_clear(key);
        free(key);
        return status;
    }

    out_scheme_key->data = key;
    out_scheme_key->n = params->n;
    return KNAP_OK;
}

static KnapStatus mh_iterated_encrypt(const SchemeKey *scheme_key,
                                      BitView message, mpz_t out_ciphertext) {
    MhIteratedKey *key;

    if (!scheme_key || !scheme_key->data || !message.data ||
        message.length == 0) {
        return KNAP_ERR_INVALID;
    }
    if (message.length != scheme_key->n) {
        return KNAP_ERR_INVALID;
    }

    key = mh_iterated_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    mh_encrypt_impl(&key->base, message, out_ciphertext);
    return KNAP_OK;
}

static KnapStatus mh_iterated_decrypt(const SchemeKey *scheme_key,
                                      const mpz_t ciphertext,
                                      BitBuf *out_message) {
    MhIteratedKey *key;
    KnapStatus status;
    mpz_t s;

    if (!scheme_key || !scheme_key->data || !out_message ||
        scheme_key->n == 0) {
        return KNAP_ERR_INVALID;
    }

    key = mh_iterated_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    status = bit_buf_alloc(out_message, (size_t)scheme_key->n);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);
        return status;
    }

    mpz_init_set(s, ciphertext);

    for (u64 i = key->layer_count; i-- > 0;) {
        mpz_mul(s, s, key->extra_layers[i].mult_inv);
        mpz_mod(s, s, key->extra_layers[i].mod);
    }

    status = mh_decrypt_impl(&key->base, s, out_message);
    mpz_clear(s);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);
        return status;
    }

    return KNAP_OK;
}

static void mh_iterated_scheme_key_clear(SchemeKey *scheme_key) {
    if (!scheme_key) {
        return;
    }

    if (scheme_key->data) {
        MhIteratedKey *key = (MhIteratedKey *)scheme_key->data;
        mh_iterated_key_clear(key);
        free(key);
    }

    scheme_key->data = NULL;
    scheme_key->n = 0;
}

const SchemeOps *scheme_mh_iterated_get(void) {
    static const SchemeOps ops = {
        .info =
            {
                .id = "mh-iterated",
                .name = "Merkle-Hellman (iterated)",
            },
        .keygen = mh_iterated_keygen,
        .encrypt = mh_iterated_encrypt,
        .decrypt = mh_iterated_decrypt,
        .scheme_key_clear = mh_iterated_scheme_key_clear,
    };

    return &ops;
}
