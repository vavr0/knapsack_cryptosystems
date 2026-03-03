#include "scheme.h"

typedef struct {
    size_t n;     // key length
    mpz_t *w;     // private key sequence (weight)
    mpz_t *b;     // public key sequence (basis)
    mpz_t m;      // modulus (m > sum(weights))
    mpz_t n_mult; // multiplier (no commond factor with m)
} MhKey;

static void mh_key_init(MhKey *key, size_t n) {
    key->n = n;
    key->w = malloc(n * sizeof(mpz_t));
    key->b = malloc(n * sizeof(mpz_t));
    for (size_t i = 0; i < n; i++) {
        mpz_init(key->w[i]);
        mpz_init(key->b[i]);
    }
    mpz_inits(key->m, key->n_mult, NULL);
}

static void mh_key_clear(MhKey *key) {
    for (size_t i = 0; i < key->n; i++) {
        mpz_clear(key->w[i]);
        mpz_clear(key->b[i]);
    }
    free(key->w);
    free(key->b);
    mpz_clears(key->m, key->n_mult, NULL);
}

static void mh_key_generate(MhKey *key) {
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

    // Choose mod > sum(W)
    mpz_add_ui(key->m, sum, 100); // plus 100

    // Choose n coprime to m
    mpz_set_ui(key->n_mult, 31); // can randomize later

    // Generate public key b_i = (w_i * n) mod m
    for (size_t i = 0; i < key->n; i++) {
        mpz_mul(key->b[i], key->w[i], key->n_mult);
        mpz_mod(key->b[i], key->b[i], key->m);
    }

    mpz_clear(sum);
}

static void mh_encrypt_impl(const MhKey *key, const i32 *message,
                            mpz_t ciphertext) {
    mpz_set_ui(ciphertext, 0);
    for (size_t i = 0; i < key->n; i++) {
        if (message[i]) {
            mpz_add(ciphertext, ciphertext, key->b[i]);
        }
    }
}

// TODO NOT VERBOSE
static void mh_decrypt_impl_verbose(const MhKey *key, const mpz_t ciphertext,
                                    i32 *message, b8 show_steps) {
    mpz_t s, n_inv;
    mpz_inits(s, n_inv, NULL);

    // Compute n_inv = n^{-1} mod m
    if (mpz_invert(n_inv, key->n_mult, key->m) == 0) {
        fprintf(stderr,
                "❌ Error: n has no modular inverse mod m (not coprime)\n");
        mpz_clears(s, n_inv, NULL);
        return;
    }

    // Compute s = (C * n_inv) mod m
    mpz_mul(s, ciphertext, n_inv);
    mpz_mod(s, s, key->m);

    if (show_steps) {
        printf("\nGreedy decryption steps:\n");
    }

    // Greedy algorithm: recover bits from largest to smallest
    for (size_t i = key->n; i-- > 0;) {

        if (mpz_cmp(s, key->w[i]) >= 0) {
            message[i] = 1;
            if (show_steps) {
                gmp_printf("i=%zu, s=%Zd, w[i]=%Zd -> take", (size_t)i, s,
                           key->w[i]);
            }
            mpz_sub(s, s, key->w[i]);
            if (show_steps) {
                gmp_printf(", s=%Zd\n", s);
            }
        } else {
            message[i] = 0;
            if (show_steps) {
                gmp_printf("i=%zu, s=%Zd, w[i]=%Zd -> skip, s=%Zd\n", (size_t)i,
                           s, key->w[i], s);
            }
        }
    }

    mpz_clears(s, n_inv, NULL);
}

static MhKey *mh_key_from_keypair(const SchemeKeypair *keypair) {
    if (!keypair || !keypair->impl) {
        return NULL;
    }
    return (MhKey *)keypair->impl;
}

static i32 mh_keygen(size_t n, SchemeKeypair *out_keypair) {
    if (!out_keypair || n == 0) {
        return -1;
    }
    out_keypair->impl = NULL;
    out_keypair->n = 0;
    MhKey *key = (MhKey *)malloc(sizeof(*key));
    if (!key) {
        return -1;
    }
    mh_key_init(key, n);
    mh_key_generate(key);
    out_keypair->impl = key;
    out_keypair->n = n;
    return 0;
}

static i32 mh_encrypt(const SchemeKeypair *keypair, const i32 *message_bits,
                      size_t n, mpz_t out_ciphertext) {
    if (!keypair || !message_bits || !keypair->impl || n == 0) {
        return -1;
    }
    if (n != keypair->n) {
        return -1;
    }
    MhKey *key = mh_key_from_keypair(keypair);
    if (!key) {
        return -1;
    }
    mh_encrypt_impl(key, message_bits, out_ciphertext);
    return 0;
}

static i32 mh_decrypt(const SchemeKeypair *keypair, const mpz_t ciphertext,
                      i32 *out_message_bits, size_t n, b8 show_steps) {
    if (!keypair || !out_message_bits || !keypair->impl || n == 0) {
        return -1;
    }
    if (n != keypair->n) {
        return -1;
    }
    MhKey *key = mh_key_from_keypair(keypair);
    if (!key) {
        return -1;
    }
    mpz_t n_inv;
    mpz_init(n_inv);
    if (mpz_invert(n_inv, key->n_mult, key->m) == 0) {
        mpz_clear(n_inv);
        return -1;
    }
    mpz_clear(n_inv);
    mh_decrypt_impl_verbose(key, ciphertext, out_message_bits, show_steps != 0);
    return 0;
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
