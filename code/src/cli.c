#include "cli.h"
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
            out->message_bits = argv[++i];
            if (!bits_is_valid(out->message_bits)) {
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
        } else if (strcmp(flag, "-h") == 0 || strcmp(flag, "--help") == 0) {
            return KNAP_STATUS_HELP;
        } else {
            return KNAP_ERR_INVALID;
        }
    }
    return KNAP_OK;
}
static KnapStatus validate_flags(const CliFlags *flags);

i32 parse_demo_options(int argc, char **argv, DemoOptions *out) {
    out->message_bits = NULL; // means interactive input (current behavior)
    out->show_steps = 0;
    out->has_seed = 0;
    out->seed = 0; // ignored unless has_seed=1

    for (i32 i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--msg") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->message_bits = argv[++i];
            if (!bits_is_valid(out->message_bits)) {
                return -1;
            }
        } else if (strcmp(arg, "--seed") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->seed = (u32)strtoul(argv[++i], NULL, 10);
            out->has_seed = 1;
        } else if (strcmp(arg, "-n") == 0) {
            out->n = argv[++i];
        } else if (strcmp(arg, "--show-steps") == 0) {
            out->show_steps = 1;

        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            return 1; // signal "help requested"
        } else {
            return -1; // unknown flag
        }
    }

    return 0;
}

i32 parse_bench_options(int argc, char **argv, BenchOptions *out) {
    // defaults
    out->n_min = 8;
    out->n_max = 20;
    out->reps = 3;
    out->has_seed = 0;
    out->seed = 0;
    out->message_bits = NULL;
    out->format = "csv";

    for (i32 i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (strcmp(arg, "--n-min") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->n_min = atoi(argv[++i]);
        } else if (strcmp(arg, "--n-max") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->n_max = atoi(argv[++i]);
        } else if (strcmp(arg, "--reps") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->reps = atoi(argv[++i]);
        } else if (strcmp(arg, "--seed") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->seed = (unsigned)strtoul(argv[++i], NULL, 10);
            out->has_seed = 1;
        } else if (strcmp(arg, "--format") == 0) {
            if (i + 1 >= argc)
                return -1;
            out->format = argv[++i];
        } else if (strcmp(arg, "--msg") == 0) {
            // TODO: make utils to make it possible to use plain text
            if (i + 1 >= argc)
                return -1;
            out->message_bits = argv[++i];
            if (!bits_is_valid(out->message_bits)) {
                return -1;
            }

        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            return 1; // signal "help requested"
        } else {
            return -1; // unknown flag
        }
    }

    if (out->n_min <= 0 || out->n_max < out->n_min || out->reps <= 0) {
        return -1;
    }
    return 0;
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
