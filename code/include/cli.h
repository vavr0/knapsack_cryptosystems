#pragma once
#include "bitvec.h"
#include "common.h"
#include "error.h"

typedef enum {
    CLI_MODE_DEMO = 0,
    CLI_MODE_BENCH,
} CliMode;

typedef struct {
    CliMode mode;
    const char *scheme_id;
    BitBuf message_bits;
    u64 seed;
    b8 has_seed;
    u64 n;
    u64 reps;
    const char *format;         // "csv" for now

} CliFlags;

KnapStatus parse_args(int argc, char **argv, CliFlags *out);
