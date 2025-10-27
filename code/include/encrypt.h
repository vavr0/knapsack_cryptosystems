#pragma once
#include "common.h"
#include "keygen.h"

void encrypt_message(const KnapsackKey *key, const int *message, mpz_t ciphertext);

