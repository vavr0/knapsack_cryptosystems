#include "cli.h"
#include "bitvec.h"
#include "error.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

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

static KnapStatus parse_flags(int argc, char **argv, CliFlags *out) {
    if (!out || argc < 2 || !argv) {
        return KNAP_STATUS_HELP;
    }

    for (i32 i = 2; i < argc; i++) {
        const char *flag = argv[i];

        if (strcmp(flag, "--msg") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            KnapStatus status =
                bit_buf_from_cstr(&out->message_bits, argv[++i]);
            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }
        } else if (strcmp(flag, "--scheme") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->scheme_id = argv[++i];
        } else if (strcmp(flag, "--seed") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->seed = (u32)strtoul(argv[++i], NULL, 10);
            out->has_seed = 1;
        } else if (strcmp(flag, "--format") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->format = argv[++i];
        } else if (strcmp(flag, "--show-steps") == 0) {
            out->show_steps = 1;
        } else if (strcmp(flag, "--n") == 0) {
            char *end = NULL;
            unsigned long long value;
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            value = strtoull(argv[++i], &end, 10);
            if (!end || *end != '\0' || value == 0) {
                return KNAP_ERR_INVALID;
            }
            out->n = (u64)value;
        } else if (strcmp(flag, "--reps") == 0) {
            char *end = NULL;
            unsigned long long value;
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            value = strtoull(argv[++i], &end, 10);
            if (!end || *end != '\0' || value == 0) {
                return KNAP_ERR_INVALID;
            }
            out->reps = (u64)value;

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

    if (flags->scheme_id && strcmp(flags->scheme_id, "mh") != 0) {
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

    if (flags->show_steps && flags->mode != CLI_MODE_DEMO) {
        return KNAP_ERR_INVALID;
    }

    if (flags->mode == CLI_MODE_DEMO) {
        if (flags->n != 0 || flags->reps != 0 || flags->format != NULL) {
            return KNAP_ERR_INVALID;
        }
    }

    if (flags->mode == CLI_MODE_BENCH) {
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
        return status;
    }
    status = parse_flags(argc, argv, out);
    if (status != KNAP_OK) {
        return status;
    }
    status = validate_flags(out);
    if (status != KNAP_OK) {
        return status;
    }

    return KNAP_OK;
}
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

static KnapStatus parse_flags(int argc, char **argv, CliFlags *out) {
    if (!out || argc < 2 || !argv) {
        return KNAP_STATUS_HELP;
    }

    for (i32 i = 2; i < argc; i++) {
        const char *flag = argv[i];

        if (strcmp(flag, "--msg") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            KnapStatus status =
                bit_buf_from_cstr(&out->message_bits, argv[++i]);
            if (status != KNAP_OK) {
                return KNAP_ERR_INVALID;
            }
        } else if (strcmp(flag, "--scheme") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->scheme_id = argv[++i];
        } else if (strcmp(flag, "--seed") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->seed = (u32)strtoul(argv[++i], NULL, 10);
            out->has_seed = 1;
        } else if (strcmp(flag, "--format") == 0) {
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            out->format = argv[++i];
        } else if (strcmp(flag, "--show-steps") == 0) {
            out->show_steps = 1;
        } else if (strcmp(flag, "--n") == 0) {
            char *end = NULL;
            unsigned long long value;
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            value = strtoull(argv[++i], &end, 10);
            if (!end || *end != '\0' || value == 0) {
                return KNAP_ERR_INVALID;
            }
            out->n = (u64)value;
        } else if (strcmp(flag, "--reps") == 0) {
            char *end = NULL;
            unsigned long long value;
            if (i + 1 >= argc) {
                return KNAP_ERR_INVALID;
            }
            value = strtoull(argv[++i], &end, 10);
            if (!end || *end != '\0' || value == 0) {
                return KNAP_ERR_INVALID;
            }
            out->reps = (u64)value;

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

    if (flags->scheme_id && strcmp(flags->scheme_id, "mh") != 0) {
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

    if (flags->show_steps && flags->mode != CLI_MODE_DEMO) {
        return KNAP_ERR_INVALID;
    }

    if (flags->mode == CLI_MODE_DEMO) {
        if (flags->n != 0 || flags->reps != 0 || flags->format != NULL) {
            return KNAP_ERR_INVALID;
        }
    }

    if (flags->mode == CLI_MODE_BENCH) {
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
        return status;
    }
    status = parse_flags(argc, argv, out);
    if (status != KNAP_OK) {
        return status;
    }
    status = validate_flags(out);
    if (status != KNAP_OK) {
        return status;
    }

    return KNAP_OK;
}
