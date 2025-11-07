#pragma once
#include "common.h"
#include "keygen.h"

// Decrypts ciphertext using private key
// 'message' must be an int array of size key->n (output bits)
void decrypt_message(const KnapsackKey *key, const mpz_t ciphertext, int *message);
