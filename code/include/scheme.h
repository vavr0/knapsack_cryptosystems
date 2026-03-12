#pragma once
#include "bitvec.h"
#include "common.h"
#include "error.h"
#include <gmp.h>

typedef struct {
    void *impl; // scheme-specific keypair/context
    u64 n;   // key/message length
} SchemeKeypair;

typedef struct {
    u64 n;
    u8 has_seed;
    u32 seed;
    u32 flags;
} SchemeKeygenParams ;

typedef struct {
    const char *id; // e.g. "mh-classic"
    const char *name;
} SchemeInfo;

typedef struct {
    SchemeInfo info;
    KnapStatus (*keygen)(const SchemeKeygenParams params, SchemeKeypair *out_keypair);
    KnapStatus (*encrypt)(const SchemeKeypair *keypair, BitView message,
                          mpz_t out_ciphertext);
    KnapStatus (*decrypt)(const SchemeKeypair *keypair, const mpz_t ciphertext,
                          BitBuf *out_message, b8 show_steps);
    void (*keypair_clear)(SchemeKeypair *keypair);
} SchemeOps;

const SchemeOps *scheme_mh_get(void);
