#include "cli.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>


i32 parse_demo_options(int argc, char **argv, DemoOptions *out) {
    out->message_bits = NULL;  // means interactive input (current behavior)
    out->show_steps = 0;
    out->has_seed = 0;
    out->seed = 0;  // ignored unless has_seed=1

    for (i32 i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--msg") == 0) {
            if (i + 1 >= argc) return -1;
            out->message_bits = argv[++i];
            if (!bits_is_valid(out->message_bits)) {
                return -1;
            }
        } else if (strcmp(arg, "--seed") == 0) {
            if (i + 1 >= argc) return -1;
            out->seed = (u32)strtoul(argv[++i], NULL, 10);
            out->has_seed = 1;
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
            if (i + 1 >= argc) return -1;
            out->n_min = atoi(argv[++i]);
        } else if (strcmp(arg, "--n-max") == 0) {
            if (i + 1 >= argc) return -1;
            out->n_max = atoi(argv[++i]);
        } else if (strcmp(arg, "--reps") == 0) {
            if (i + 1 >= argc) return -1;
            out->reps = atoi(argv[++i]);
        } else if (strcmp(arg, "--seed") == 0) {
            if (i + 1 >= argc) return -1;
            out->seed = (unsigned)strtoul(argv[++i], NULL, 10);
            out->has_seed = 1;
        } else if (strcmp(arg, "--format") == 0) {
            if (i + 1 >= argc) return -1;
            out->format = argv[++i];
        } else if (strcmp(arg, "--msg") == 0) {
            // TODO: make utils to make it possible to use plain text
            if (i + 1 >= argc) return -1;
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
