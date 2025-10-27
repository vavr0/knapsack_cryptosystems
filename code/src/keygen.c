#include "keygen.h"

void keygen_init(KnapsackKey *key, size_t n) {
    key->n = n;
    key->w = malloc(n * sizeof(mpz_t));
    key->b = malloc(n * sizeof(mpz_t));
    for (size_t i = 0; i < n; i++) {
        mpz_init(key->w[i]);
        mpz_init(key->b[i]);
    }
    mpz_inits(key->m, key->n_mult, NULL);
}

void keygen_generate(KnapsackKey *key) {
    // Build superinceasing sequencce
    mpz_set_ui(key->w[0], 2);
    for (size_t i = 1; i < key->n; i++) {
        mpz_mul_ui(key->w[i], key->w[i - 1], 2); // times 2
        mpz_add_ui(key->w[i], key->w[i], 1); // plus 1
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

    printf("✅ Keys generated: %zu elements\n", key->n);
}

void keygen_clear(KnapsackKey *key) {
    for (size_t i = 0; i < key->n; i++) {
        mpz_clear(key->w[i]);
        mpz_clear(key->b[i]);
    }
    free(key->w);
    free(key->b);
    mpz_clears(key->m, key->n_mult, NULL);
}

