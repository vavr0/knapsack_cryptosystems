#pragma once
#include "common.h"
#include <gmp.h>

typedef struct {
    void *impl; // scheme-specific keypair/context
    size_t n;   // key/message length
} SchemeKeypair;

typedef struct {
    const char *id; // e.g. "mh-classic"
    const char *name;
} SchemeInfo;

typedef struct {
    SchemeInfo info;
    // Allocate + generate keypair for n bits
    // Returns 0 on success, -1 on failure.
    i32 (*keygen)(size_t n, SchemeKeypair *out_keypair);
    // Encrypt n-bit message into ciphertext
    // Returns 0 on success, -1 on failure.
    i32 (*encrypt)(const SchemeKeypair *keypair, const i32 *message_bits,
                   size_t n, mpz_t out_ciphertext);
    // Decrypt ciphertext into n-bit message
    // show_steps: 0/1 for optional verbose output
    // Returns 0 on success, -1 on failure.
    i32 (*decrypt)(const SchemeKeypair *keypair, const mpz_t ciphertext,
                   i32 *out_message_bits, size_t n, b8 show_steps);
    // Free all resources held by keypair (safe on empty keypair)
    void (*keypair_clear)(SchemeKeypair *keypair);
} SchemeOps;
//
//
// First implementation (Merkle-Hellman)
const SchemeOps *scheme_mh_get(void);


