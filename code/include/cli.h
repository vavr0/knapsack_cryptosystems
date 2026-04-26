#pragma once
#include "buffer.h"
#include "common.h"
#include "error.h"

typedef enum {
    CLI_INPUT_NONE = 0,
    CLI_INPUT_BITS,
    CLI_INPUT_TEXT,
} CliInputMode;

typedef enum {
    CLI_MODE_DEMO = 0,
    CLI_MODE_BENCH,
} CliMode;

typedef struct {
    CliMode mode;
    CliInputMode input_mode;
    const char *scheme_id;
    TextBuf text_message;
    BitBuf bits_message;
    u64 seed;
    b8 has_seed;
    u64 n;
    u64 reps;
    const char *format; // "csv" for now

} CliFlags;

KnapStatus parse_args(int argc, char **argv, CliFlags *out);
