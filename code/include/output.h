#pragma once

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
