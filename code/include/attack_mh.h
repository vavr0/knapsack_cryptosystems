#pragma once

#include "buffer.h"
#include "cli.h"
#include "common.h"
#include "error.h"
#include <gmp.h>

typedef struct {
    u64 n;
    const mpz_t *weights;
} MhPublicKeyView;

typedef struct {
    BitBuf message;
    u64 checked_count;
    b8 success;
} AttackResult;

void attack_result_clear(AttackResult *result);
KnapStatus attack_mh_run(CliFlags *flags);
