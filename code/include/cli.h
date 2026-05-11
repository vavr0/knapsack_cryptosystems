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
    CLI_MODE_ATTACK,
} CliMode;

typedef struct {
    CliMode mode;
    CliInputMode input_mode;
    const char *scheme_id;
    const char *attack_id;
    TextBuf text_message;
    BitBuf bits_message;
    u64 seed;
    b8 has_seed;
    u64 n;
    u64 reps;
} CliFlags;

KnapStatus parse_args(int argc, char **argv, CliFlags *out);
void cli_flags_clear(CliFlags *flags);
