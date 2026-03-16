#include "bitvec.h"
#include "common.h"
#include "error.h"
#include "scheme.h"
#include <gmp.h>

// TODO
// - mh_key_init(...) still does unchecked malloc for w / b
// - decrypt helper still prints steps internally

typedef struct {
    u64 n;        // key length
    mpz_t *w;     // private key sequence (weight)
    mpz_t *b;     // public key sequence (basis)
    mpz_t m;      // modulus (m > sum(weights))
    mpz_t n_mult; // multiplier (no commond factor with m)
    mpz_t n_inv;  // inverse
} MhKey;

static KnapStatus mh_key_init(MhKey *key, u64 n) {
    u64 i;
    if (!key || n == 0) {
        return KNAP_ERR_INVALID;
    }
    key->n = 0;
    key->w = NULL;
    key->b = NULL;
    key->w = malloc((size_t)n * sizeof(*key->w));
    if (!key->w) {
        return KNAP_ERR_ALLOC;
    }
    key->b = malloc((size_t)n * sizeof(*key->b));
    if (!key->b) {
        free(key->w);
        key->w = NULL;
        return KNAP_ERR_ALLOC;
    }
    key->n = n;
    for (i = 0; i < n; i++) {
        mpz_init(key->w[i]);
        mpz_init(key->b[i]);
    }
    mpz_inits(key->m, key->n_mult, key->n_inv, NULL);
    return KNAP_OK;
}
static void mh_key_clear(MhKey *key) {
    for (size_t i = 0; i < key->n; i++) {
        mpz_clear(key->w[i]);
        mpz_clear(key->b[i]);
    }
    free(key->w);
    free(key->b);
    mpz_clears(key->m, key->n_mult, key->n_inv, NULL);
}

static KnapStatus mh_key_generate(MhKey *key) {
    // Build superinceasing sequencce
    mpz_set_ui(key->w[0], 2);
    for (size_t i = 1; i < key->n; i++) {
        mpz_mul_ui(key->w[i], key->w[i - 1], 2); // times 2
        mpz_add_ui(key->w[i], key->w[i], 1);     // plus 1
    }

    // Compute sum(W)
    mpz_t sum;
    mpz_init(sum);
    for (size_t i = 0; i < key->n; i++)
        mpz_add(sum, sum, key->w[i]);

    // Choose prime mod > sum(W)
    mpz_nextprime(key->m, sum);
    // Choose multiplier coprime to m
    mpz_set_ui(key->n_mult, 2);

    if (mpz_invert(key->n_inv, key->n_mult, key->m) == 0) {
        mpz_clear(sum);

        return KNAP_ERR_CRYPTO;
    }

    // Generate public key b_i = (w_i * n) mod m
    for (size_t i = 0; i < key->n; i++) {
        mpz_mul(key->b[i], key->w[i], key->n_mult);
        mpz_mod(key->b[i], key->b[i], key->m);
    }
    mpz_clear(sum);

    return KNAP_OK;
}

static void mh_encrypt_impl(const MhKey *key, BitView message,
                            mpz_t ciphertext) {
    mpz_set_ui(ciphertext, 0);
    for (u64 i = 0; i < message.length; i++) {
        if (message.data[i]) {
            mpz_add(ciphertext, ciphertext, key->b[i]);
        }
    }
}

// TODO NOT VERBOSE
static KnapStatus mh_decrypt_impl(const MhKey *key, const mpz_t ciphertext,
                                  BitBuf *message, b8 show_steps) {
    mpz_t s;
    mpz_init(s);

    // Compute s = (C * n_inv) mod m
    mpz_mul(s, ciphertext, key->n_inv);
    mpz_mod(s, s, key->m);

    if (show_steps) {
        printf("\nGreedy decryption steps:\n");
    }

    // Greedy algorithm: recover bits from largest to smallest
    for (size_t i = key->n; i-- > 0;) {

        if (mpz_cmp(s, key->w[i]) >= 0) {
            message->data[i] = 1;
            if (show_steps) {
                gmp_printf("i=%zu, s=%Zd, w[i]=%Zd -> take", (size_t)i, s,
                           key->w[i]);
            }

            mpz_sub(s, s, key->w[i]);
            if (show_steps) {
                gmp_printf(", s=%Zd\n", s);
            }
        } else {
            message->data[i] = 0;
            if (show_steps) {
                gmp_printf("i=%zu, s=%Zd, w[i]=%Zd -> skip, s=%Zd\n", (size_t)i,
                           s, key->w[i], s);
            }
        }
    }
    mpz_clear(s);

    return KNAP_OK;
}

static MhKey *mh_key_from_keypair(const SchemeKeypair *keypair) {
    if (!keypair || !keypair->impl) {
        return NULL;
    }
    return (MhKey *)keypair->impl;
}

static KnapStatus mh_keygen(const SchemeKeygenParams *params,
                            SchemeKeypair *out_keypair) {
    if (!params || !out_keypair || params->n == 0) {
        return KNAP_ERR_INVALID;
    }
    out_keypair->impl = NULL;
    out_keypair->n = 0;
    MhKey *key = (MhKey *)malloc(sizeof(*key));
    if (!key) {
        return KNAP_ERR_ALLOC;
    }
    KnapStatus status = mh_key_init(key, params->n);
    if (status != KNAP_OK) {
        free(key);
        return status;
    }

    status = mh_key_generate(key);
    if (status != KNAP_OK) {
        mh_key_clear(key);
        free(key);

        return status;
    }
    out_keypair->impl = key;
    out_keypair->n = params->n;

    return KNAP_OK;
}

static KnapStatus mh_encrypt(const SchemeKeypair *keypair, BitView message,
                             mpz_t out_ciphertext) {
    if (!keypair || !keypair->impl || message.length == 0 || !message.data) {
        return KNAP_ERR_INVALID;
    }

    if (message.length != keypair->n) {
        return KNAP_ERR_INVALID;
    }

    MhKey *key = mh_key_from_keypair(keypair);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }
    mh_encrypt_impl(key, message, out_ciphertext);
    return KNAP_OK;
}

static KnapStatus mh_decrypt(const SchemeKeypair *keypair,
                             const mpz_t ciphertext, BitBuf *out_message,
                             b8 show_steps) {
    KnapStatus status;
    if (!keypair || !out_message || !keypair->impl || keypair->n == 0) {
        return KNAP_ERR_INVALID;
    }

    MhKey *key = mh_key_from_keypair(keypair);
    if (!key) {
        return KNAP_ERR_INTERNAL;
    }

    status = bit_buf_alloc(out_message, keypair->n);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);

        return status;
    }

    status = mh_decrypt_impl(key, ciphertext, out_message, show_steps != 0);
    if (status != KNAP_OK) {
        bit_buf_clear(out_message);

        return status;
    }
    return KNAP_OK;
}

static void mh_keypair_clear(SchemeKeypair *keypair) {
    if (!keypair) {
        return;
    }
    if (keypair->impl) {
        MhKey *key = (MhKey *)keypair->impl;
        mh_key_clear(key);
        free(key);
    }
    keypair->impl = NULL;
    keypair->n = 0;
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
        .keypair_clear = mh_keypair_clear,
    };
    return &ops;
}
