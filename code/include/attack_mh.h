#pragma once

#include "buffer.h"
#include "cli.h"
#include "common.h"
#include "error.h"
#include <gmp.h>

#define ATTACK_BRUTE_MAX_N 30u
#define ATTACK_MITM_MAX_HALF_N 20u

// Public subset-sum instance used by 
// MH attacks.
typedef struct {
    u64 n;
    const mpz_t *weights;
} MhPublicKeyView;

// Result and measured work counters 
// for one attack run.
typedef struct {
    BitBuf message;
    u64 checked_count;
    u64 table_entries;
    b8 success;
} AttackResult;

// Meet-in-the-middle table entry.
typedef struct {
    mpz_t value;
    u64 mask;
} MitmEntry;

void attack_result_clear(AttackResult *result);

// Run a classic MH subset-sum solver.
KnapStatus attack_mh_run(CliFlags *flags);
