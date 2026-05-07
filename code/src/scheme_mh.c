#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
#include "scheme_mh_common.h"
#include <stddef.h>

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

    status = mh_key_build_private(key, &rng);
    if (status != KNAP_OK) {
        mh_key_clear(key);
        free(key);
        return status;
    }

    mh_key_build_public(key, NULL);

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
