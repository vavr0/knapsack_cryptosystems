#include "buffer.h"
#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
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
    u64 n;
    u64 layer_count;
    mpz_t *priv_weights;
    mpz_t *pub_weights;
    MhIteratedLayer *layers;
} MhIteratedKey;

static KnapStatus mh_iterated_key_alloc(MhIteratedKey *key, u64 n,
                                        u64 layer_count) {
    (void)key;
    (void)n;
    (void)layer_count;
    return KNAP_ERR_INTERNAL;
}

static void mh_iterated_key_clear(MhIteratedKey *key) {
    (void)key;
}

static KnapStatus mh_iterated_key_build(MhIteratedKey *key, PrngState *rng) {
    (void)key;
    (void)rng;
    return KNAP_ERR_INTERNAL;
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
                                      BitView message,
                                      mpz_t out_ciphertext) {
    MhIteratedKey *key;

    (void)out_ciphertext;
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

    return KNAP_ERR_INTERNAL;
}

static KnapStatus mh_iterated_decrypt(const SchemeKey *scheme_key,
                                      const mpz_t ciphertext,
                                      BitBuf *out_message) {
    MhIteratedKey *key;

    (void)ciphertext;
    if (!scheme_key || !scheme_key->data || !out_message ||
        scheme_key->n == 0) {
        return KNAP_ERR_INVALID;
    }

    key = mh_iterated_key_from_scheme_key(scheme_key);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    return KNAP_ERR_INTERNAL;
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
