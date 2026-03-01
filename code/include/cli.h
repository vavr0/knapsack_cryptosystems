#pragma once
#include "common.h"

typedef struct {
    const char *message_bits;   // NULL => interactive/default
    b8 show_steps;             // 0/1
    b8 has_seed;               // 0/1
    u32 seed;
} DemoOptions;

typedef struct {
    i32 n_min;
    i32 n_max;
    i32 reps;
    b8 has_seed;               // 0/1
    u32 seed;
    const char *format;         // "csv" for now
    const char *message_bits;
} BenchOptions;

i32 parse_demo_options(int argc, char **argv, DemoOptions *out);
i32 parse_bench_options(int argc, char **argv, BenchOptions *out);
