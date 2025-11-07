#include "decrypt.h"

void decrypt_message(const KnapsackKey *key, const mpz_t ciphertext, int *message) {
    mpz_t s, n_inv;
    mpz_inits(s, n_inv, NULL);

    // Compute n_inv = n^{-1} mod m
    if (mpz_invert(n_inv, key->n_mult, key->m) == 0) {
        fprintf(stderr, "❌ Error: n has no modular inverse mod m (not coprime)\n");
        mpz_clears(s, n_inv, NULL);
        return;
    }

    // Compute s = (C * n_inv) mod m
    mpz_mul(s, ciphertext, n_inv);
    mpz_mod(s, s, key->m);

    // Greedy algorithm: recover bits from largest to smallest
    for (ssize_t i = key->n - 1; i >= 0; i--) {
        if (mpz_cmp(s, key->w[i]) >= 0) {
            message[i] = 1;
            mpz_sub(s, s, key->w[i]);
        } else {
            message[i] = 0;
        }
    }

    mpz_clears(s, n_inv, NULL);
    printf("✅ Message decrypted.\n");
}
