/*
 * Based on PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include "common.h"

typedef struct {
    u64 state;
    u64 inc;
} PrngState;

void prng_seed(PrngState *rng, u64 initstate, u64 initseq);
u32 prng_rand(PrngState *rng);
u64 prng_rand_u64(PrngState *rng);
