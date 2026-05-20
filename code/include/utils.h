#pragma once

#include "buffer.h"
#include "common.h"
#include "error.h"
#include "rand.h"

f64 now_ms(void);
KnapStatus fill_message_random(BitBuf *bits_message, u64 n, PrngState *rng);
