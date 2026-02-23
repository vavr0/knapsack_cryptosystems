#include <stdio.h>
#include <string.h>
#include "app.h"


static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s demo  [options]\n", prog);
    fprintf(stderr, "  %s bench [options]\n", prog);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    const char *mode = argv[1];
    if (strcmp(mode, "demo") == 0) {
        return run_demo(argc - 1, argv + 1);
    }
    if (strcmp(mode, "bench") == 0) {
        return run_bench(argc - 1, argv + 1);
    }
    if (strcmp(mode, "--help") == 0 || strcmp(mode, "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    fprintf(stderr, "Unknown mode: %s\n", mode);
    print_usage(argv[0]);
    return 1;
}
