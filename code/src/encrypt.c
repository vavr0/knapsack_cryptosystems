#include "encrypt.h"

void encrypt_message(const KnapsackKey *key, const int *message, mpz_t ciphertext) {
    mpz_set_ui(ciphertext, 0);
    for (size_t i = 0; i < key->n; i++) {
        if (message[i]) {
            mpz_add(ciphertext, ciphertext, key->b[i]);
        }
    }
}

