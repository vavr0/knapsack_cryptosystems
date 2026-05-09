#pragma once

#include "attack_mh.h"
#include "buffer.h"
#include "cli.h"
#include "error.h"
#include "plaintext.h"
#include "scheme.h"
#include <gmp.h>

void print_usage(const char *prog);
KnapStatus print_demo_result(const CliFlags *flags, const SchemeOps *scheme,
                             const BitBlocks *blocks, const mpz_t *ciphertexts,
                             const BitBuf *decrypted_bits);
void print_attack_result(const char *attack_id, const SchemeOps *scheme, u64 n,
                         const u64 seed[2], const AttackResult *result,
                         f64 attack_ms);
