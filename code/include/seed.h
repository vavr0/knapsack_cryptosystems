#pragma once

#include "common.h"
#include "error.h"
#include <stddef.h>

// Fill buf with operating-system entropy.
KnapStatus seed_get_entropy(void *buf, size_t len);

// Expand one CLI seed into two PCG 
// initialization values.
void seed_expand_u64(u64 seed, u64 out[2]);

// Resolve deterministic or random seed pair for one run.
KnapStatus seed_resolve_pair(b8 has_seed, u64 seed, u64 out[2]);
