#include "cli.h"
#include "buffer.h"
#include "error.h"
#include "scheme.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static void text_buf_clear(TextBuf *buf) {
    if (!buf) {
        return;
    }

    free(buf->data);
    *buf = (TextBuf){0};
}

static KnapStatus text_buf_from_cstr(TextBuf *out, const char *s) {
    size_t len;
    char *copy;

    if (!out || !s) {
        return KNAP_ERR_INVALID;
    }

    len = strlen(s);
    if (len == 0) {
        return KNAP_ERR_INVALID;
    }

    copy = malloc(len + 1);
    if (!copy) {
        return KNAP_ERR_ALLOC;
    }

    memcpy(copy, s, len + 1);
    text_buf_clear(out);
    out->data = copy;
    out->length = (u64)len;
    return KNAP_OK;
}

static void cli_flags_clear(CliFlags *flags) {
    if (!flags) {
        return;
    }

    bit_buf_clear(&flags->message_bits);
    text_buf_clear(&flags->message_text);
    flags->input_mode = CLI_INPUT_NONE;
}

static KnapStatus parse_mode(int argc, char **argv, CliFlags *out) {
    if (!out || argc < 2 || !argv) {
        return KNAP_STATUS_HELP;
    }
    const char *mode = argv[1];
    if (strcmp(mode, "demo") == 0) {
        out->mode = CLI_MODE_DEMO;
        return KNAP_OK;
    }
    if (strcmp(mode, "bench") == 0) {
        out->mode = CLI_MODE_BENCH;
        return KNAP_OK;
    }
    if (strcmp(mode, "--help") == 0 || strcmp(mode, "-h") == 0) {
        return KNAP_STATUS_HELP;
    }

    return KNAP_ERR_INVALID;
}

static KnapStatus parse_u64_str(const char *s, u64 *out, b8 allow_zero) {
    char *end = NULL;
    unsigned long long value;

    if (!s || !out || s[0] == '\0' || s[0] == '-' || s[0] == '+') {
        return KNAP_ERR_INVALID;
    }

    errno = 0;
    value = strtoull(s, &end, 10);
    if (errno != 0 || end == s || *end != '\0') {
        return KNAP_ERR_INVALID;
    }
    if (!allow_zero && value == 0) {
        return KNAP_ERR_INVALID;
    }

    *out = (u64)value;
    return KNAP_OK;
}

static KnapStatus parse_flags(int argc, char **argv, CliFlags *out) {
    KnapStatus status;

    if (!out || argc < 2 || !argv) {
        return KNAP_STATUS_HELP;
    }

    for (i32 i = 2; i < argc; i++) {
        const char *flag = argv[i];

        if (strcmp(flag, "--bits") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }

            if (out->input_mode != CLI_INPUT_NONE) {
                return KNAP_ERR_INVALID;
            }

            status = bit_buf_from_cstr(&out->message_bits, argv[++i]);
            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }
            out->input_mode = CLI_INPUT_BITS;
        } else if (strcmp(flag, "--msg") == 0) {
            if (i + 1 >= argc || out->mode != CLI_MODE_DEMO) {
                return KNAP_ERR_INVALID;
            }

            if (out->input_mode != CLI_INPUT_NONE) {
                return KNAP_ERR_INVALID;
            }

            status = text_buf_from_cstr(&out->message_text, argv[++i]);
            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }
            out->input_mode = CLI_INPUT_TEXT;
        } else if (strcmp(flag, "--scheme") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->scheme_id = argv[++i];
        } else if (strcmp(flag, "--seed") == 0) {
            u64 value;
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            status = parse_u64_str(argv[++i], &value, 1);

            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }
            out->seed = value;
            out->has_seed = 1;
        } else if (strcmp(flag, "--format") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->format = argv[++i];
        }  else if (strcmp(flag, "--n") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }

            status = parse_u64_str(argv[++i], &out->n, 0);
            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }
        } else if (strcmp(flag, "--reps") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }

            status = parse_u64_str(argv[++i], &out->reps, 0);
            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }

        } else if (strcmp(flag, "-h") == 0 || strcmp(flag, "--help") == 0) {
            return KNAP_STATUS_HELP;
        } else {
            return KNAP_ERR_INVALID;
        }
    }
    return KNAP_OK;
}

static KnapStatus validate_flags(const CliFlags *flags) {
    if (!flags) {
        return KNAP_ERR_INVALID;
    }

    if (flags->mode != CLI_MODE_DEMO && flags->mode != CLI_MODE_BENCH) {
        return KNAP_ERR_INVALID;
    }

    if (flags->scheme_id && scheme_resolve(flags->scheme_id) == NULL) {
        return KNAP_ERR_INVALID;
    }

    if (flags->format) {
        if (strcmp(flags->format, "csv") != 0) {
            return KNAP_ERR_INVALID;
        }
        if (flags->mode != CLI_MODE_BENCH) {
            return KNAP_ERR_INVALID;
        }
    }

    if (flags->input_mode == CLI_INPUT_BITS) {
        if (flags->message_bits.length == 0 || flags->message_text.data != NULL ||
            flags->message_text.length != 0) {
            return KNAP_ERR_INVALID;
        }
    } else if (flags->input_mode == CLI_INPUT_TEXT) {
        if (flags->message_text.data == NULL || flags->message_text.length == 0 ||
            flags->message_bits.length != 0) {
            return KNAP_ERR_INVALID;
        }
    } else if (flags->input_mode == CLI_INPUT_NONE) {
        if (flags->message_bits.length != 0 || flags->message_text.data != NULL ||
            flags->message_text.length != 0) {
            return KNAP_ERR_INVALID;
        }
    } else {
        return KNAP_ERR_INVALID;
    }

    if (flags->mode == CLI_MODE_DEMO) {
        if (flags->reps != 0 || flags->format != NULL) {
            return KNAP_ERR_INVALID;
        }

        if (flags->input_mode == CLI_INPUT_BITS ||
            flags->input_mode == CLI_INPUT_NONE) {
            if (flags->n != 0) {
                return KNAP_ERR_INVALID;
            }
        } else if (flags->input_mode == CLI_INPUT_TEXT) {
            if (flags->n == 0 || (flags->n % 8u) != 0) {
                return KNAP_ERR_INVALID;
            }
        }
    }

    if (flags->mode == CLI_MODE_BENCH) {
        if (flags->input_mode == CLI_INPUT_TEXT) {
            return KNAP_ERR_INVALID;
        }
        if (flags->message_bits.length == 0 && flags->n == 0) {
            return KNAP_ERR_INVALID;
        }
    }

    // TODO check from utils ig
    if (!bit_buf_is_valid(&flags->message_bits)) {
        return KNAP_ERR_INVALID;
    }

    return KNAP_OK;
}

KnapStatus parse_args(int argc, char **argv, CliFlags *out) {
    KnapStatus status;
    if (!out || argc <= 0 || !argv) {
        return KNAP_ERR_INVALID;
    }
    *out = (CliFlags){0};
    status = parse_mode(argc, argv, out);
    if (status != KNAP_OK) {
        cli_flags_clear(out);
        return status;
    }
    status = parse_flags(argc, argv, out);
    if (status != KNAP_OK) {
        cli_flags_clear(out);
        return status;
    }
    status = validate_flags(out);
    if (status != KNAP_OK) {
        cli_flags_clear(out);
        return status;
    }

    return KNAP_OK;
}
