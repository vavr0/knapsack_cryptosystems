#pragma once
#include "common.h"
#include "keygen.h"

void decrypt_message(const KnapsackKey *key, const mpz_t ciphertext, int message[8]);

