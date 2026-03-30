/*
 * Based on PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include "rand.h"

void prng_seed(PrngState *rng, u64 initstate, u64 initseq) {
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    prng_rand(rng);
    rng->state += initstate;
    prng_rand(rng);
}

u32 prng_rand(PrngState *rng) {
    u64 oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

u64 prng_rand_u64(PrngState *rng) {
    u64 hi = (u64)prng_rand(rng);
    u64 lo = (u64)prng_rand(rng);
    return (hi << 32) | lo;
}
