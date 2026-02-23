#pragma once
#include "common.h"
#include "keygen.h"
#include <stdbool.h>

// Decrypts ciphertext using private key
// 'message' must be an int array of size key->n (output bits)
void decrypt_message(const KnapsackKey *key, const mpz_t ciphertext, int *message);

// Verbose decryption with optional step-by-step output
void decrypt_message_verbose(const KnapsackKey *key, const mpz_t ciphertext, int *message, bool show_steps);
