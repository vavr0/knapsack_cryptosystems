#include "cli.h"
#include <string.h>
#include <stdlib.h>


static int is_bitstring(const char *s) {
    if (!s || s[0] == '\0') {
        return 0;
    }
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] != '0' && s[i] != '1') {
            return 0;
        }
    }
    return 1;
}


int parse_demo_options(int argc, char **argv, DemoOptions *out) {
    out->message_bits = NULL;  // means interactive input (current behavior)
    out->show_steps = 0;
    out->has_seed = 0;
    out->seed = 0;  // ignored unless has_seed=1

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--message") == 0) {
            if (i + 1 >= argc) return -1;
            out->message_bits = argv[++i];
            if (!is_bitstring(out->message_bits)) {
                return -1;
            }
        } else if (strcmp(arg, "--seed") == 0) {
            if (i + 1 >= argc) return -1;
            out->seed = (unsigned int)strtoul(argv[++i], NULL, 10);
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

int parse_bench_options(int argc, char **argv, BenchOptions *out) {
    // defaults
    out->n_min = 8;
    out->n_max = 20;
    out->reps = 3;
    out->has_seed = 0;
    out->seed = 0;
    out->format = "csv";
    out->kind = "pipeline";

    for (int i = 1; i < argc; i++) {
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
        } else if (strcmp(arg, "--kind") == 0) {
            if (i + 1 >= argc) return -1;
            out->kind = argv[++i];
            if (strcmp(out->kind, "pipeline") != 0 && strcmp(out->kind, "compare") != 0) {
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
