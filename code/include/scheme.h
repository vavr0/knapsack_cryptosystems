#pragma once
#include "bitvec.h"
#include "common.h"
#include "error.h"
#include <gmp.h>

typedef struct {
    void *data; // scheme-specific keypair/context
    u64 n;   // key/message length
} SchemeKey;

typedef struct {
    u64 n;
    u64 initstate;
    u64 initseq;
    u32 flags;
} SchemeKeygenParams;

typedef struct {
    const char *id; // e.g. "mh-classic"
    const char *name;
} SchemeInfo;

typedef struct {
    SchemeInfo info;
    KnapStatus (*keygen)(const SchemeKeygenParams *params, SchemeKey *out_keypair);
    KnapStatus (*encrypt)(const SchemeKey *keypair, BitView message,
                          mpz_t out_ciphertext);
    KnapStatus (*decrypt)(const SchemeKey *keypair, const mpz_t ciphertext,
                          BitBuf *out_message);
    void (*scheme_key_clear)(SchemeKey *keypair);
} SchemeOps;

const SchemeOps *scheme_mh_get(void);
const SchemeOps *scheme_mh_permuted_get(void);
const SchemeOps *scheme_resolve(const char *id);
