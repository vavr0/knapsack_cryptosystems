#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
#include "scheme_mh_common.h"

typedef struct {
    MhKey key;
    u64 *perm;
} MhPermutedKey;

static KnapStatus mh_permuted_key_alloc(MhPermutedKey *key, u64 n) {
    KnapStatus status;
    if (!key || n == 0) {
        return KNAP_ERR_INVALID;
    }

    if (n > (u64)SIZE_MAX || (size_t)n > SIZE_MAX / sizeof(*key->perm)) {
        return KNAP_ERR_ALLOC;
    }

    *key = (MhPermutedKey){0};

    status = mh_key_alloc(&key->key, n);
    if (status != KNAP_OK) {
        return status;
    }

    key->perm = malloc((size_t)n * sizeof(*key->perm));
    if (!key->perm) {
        mh_key_clear(&key->key);
        return KNAP_ERR_ALLOC;
    }

    return KNAP_OK;
}

static void mh_permuted_key_clear(MhPermutedKey *key) {
    if (!key) {
        return;
    }

    mh_key_clear(&key->key);
    free(key->perm);
    *key = (MhPermutedKey){0};
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
    KnapStatus status;
    if (!key || !rng || key->key.n == 0 || !key->perm) {
        return KNAP_ERR_INVALID;
    }

    status = mh_key_build_private(&key->key, rng);
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < key->key.n; i++) {
        key->perm[i] = i;
    }

    shuffle_u64(key->perm, key->key.n, rng);
    mh_key_build_public(&key->key, key->perm);

    return KNAP_OK;
}

static KnapStatus mh_permuted_decrypt_impl(const MhPermutedKey *key,
                                           const mpz_t ciphertext,
                                           BitBuf *message) {
    KnapStatus status;
    BitBuf private_bits = {0};

    status = bit_buf_alloc(&private_bits, key->key.n);
    if (status != KNAP_OK) {
        return status;
    }

    status = mh_decrypt_impl(&key->key, ciphertext, &private_bits);
    if (status != KNAP_OK) {
        bit_buf_clear(&private_bits);
        return status;
    }

    for (u64 i = 0; i < key->key.n; i++) {
        message->data[i] = private_bits.data[key->perm[i]];
    }

    bit_buf_clear(&private_bits);
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

    mh_encrypt_impl(&key->key, message, out_ciphertext);
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
