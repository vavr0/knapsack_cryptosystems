#pragma once

#include "common.h"
#include "error.h"
#include <stddef.h>


KnapStatus seed_get_entropy(void *buf, size_t len);
void seed_expand_u64(u64 seed, u64 out[2]);
KnapStatus seed_resolve_pair(b8 has_seed, u64 seed, u64 out[2]);
