typedef struct {
    const char *message_bits;   // NULL => interactive/default
    int show_steps;             // 0/1
    int has_seed;               // 0/1
    unsigned int seed;
} DemoOptions;

typedef struct {
    int n_min;
    int n_max;
    int reps;
    int has_seed;               // 0/1
    unsigned int seed;
    const char *format;         // "csv" for now
    const char *kind;           // "pipeline" or "compare"
} BenchOptions;

int parse_demo_options(int argc, char **argv, DemoOptions *out);
int parse_bench_options(int argc, char **argv, BenchOptions *out);
