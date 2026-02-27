#include "bench.h"
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int n;
    int *w;
    int target;
} CompareInstance;

static double now_ms_compare(void) {
    return 1000.0 * (double)clock() / (double)CLOCKS_PER_SEC;
}

static void free_compare_instance(CompareInstance *inst) {
    free(inst->w);
    inst->w = NULL;
    inst->n = 0;
    inst->target = 0;
}

static int generate_general_instance(CompareInstance *inst, int n, int max_w) {
    inst->n = n;
    inst->w = malloc((size_t)n * sizeof(int));
    if (!inst->w) {
        return -1;
    }
    for (int i = 0; i < n; i++) {
        inst->w[i] = 1 + rand() % max_w;
    }
    int target = 0;
    for (int i = 0; i < n; i++) {
        if (rand() & 1) {
            target += inst->w[i];
        }
    }
    inst->target = target;
    return 0;
}
static int generate_super_instance(CompareInstance *inst, int n,
                                   int base_step) {
    inst->n = n;
    inst->w = malloc((size_t)n * sizeof(int));
    if (!inst->w) {
        return -1;
    }
    int sum = 0;
    for (int i = 0; i < n; i++) {
        int increment = 1 + rand() % base_step;
        inst->w[i] = sum + increment;
        sum += inst->w[i];
    }
    int target = 0;
    for (int i = 0; i < n; i++) {
        if (rand() & 1) {
            target += inst->w[i];
        }
    }
    inst->target = target;
    return 0;
}

static int solve_bruteforce(const CompareInstance *inst) {
    int n = inst->n;
    int target = inst->target;
    unsigned long long total_masks = 1ULL << n;
    for (unsigned long long mask = 0; mask < total_masks; mask++) {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1ULL << i)) {
                sum += inst->w[i];
            }
        }
        if (sum == target) {
            return 1;
        }
    }
    return 0;
}
static int solve_super_greedy(const CompareInstance *inst) {
    int s = inst->target;
    for (int i = inst->n - 1; i >= 0; i--) {
        if (inst->w[i] <= s) {
            s -= inst->w[i];
        }
    }
    return (s == 0);
}

int bench_run_pipeline_csv(int n_min, int n_max, int reps, unsigned int seed) {
    for (int n = n_min; n <= n_max; n++) {
        double sum_keygen = 0.0;
        double sum_encrypt = 0.0;
        double sum_decrypt = 0.0;

        for (int r = 0; r < reps; r++) {
            int *message = malloc((size_t)n * sizeof(int));

            if (!message) {
                fprintf(stderr, "malloc failed\n");
                free(message);
                return -1;
            }

            // create message from seed
            for (int i = 0; i < n; i++) {
                message[i] = rand() & 1;
            }

            KnapsackRunRequest req;
            KnapsackRunOutput out;
            KnapsackRunMetrics metrics;

            req.n = (size_t)n;
            req.message_bits = message;
            req.show_steps = 0;
            req.capture_details = 0;

            if (knapsack_run_once(&req, &out, &metrics) != 0) {
                fprintf(stderr, "roundtrip failed at n=%d rep=%d\n", n, r);
                free(message);
                return -1;
            }

            sum_keygen += metrics.keygen_ms;
            sum_encrypt += metrics.encrypt_ms;
            sum_decrypt += metrics.decrypt_ms;

            knapsack_run_output_clear(&out);
            free(message);
        }
        double keygen_ms = sum_keygen / reps;
        double encrypt_ms = sum_encrypt / reps;
        double decrypt_ms = sum_decrypt / reps;
        double total_ms = keygen_ms + encrypt_ms + decrypt_ms;
        printf("%d,%.6f,%.6f,%.6f,%.6f,%d,%u\n", n, keygen_ms, encrypt_ms,
               decrypt_ms, total_ms, reps, seed);
    }
    return 0;
}

int bench_run_compare_csv(int n_min, int n_max, int reps, unsigned int seed) {
    if (n_max > 30) {
        fprintf(stderr, "compare mode supports n <= 30 (bruteforce limit)\n");
        return -1;
    }
    for (int n = n_min; n <= n_max; n++) {
        double sum_brute = 0.0;
        double sum_super = 0.0;
        for (int r = 0; r < reps; r++) {
            CompareInstance gen = {0};
            CompareInstance sup = {0};
            if (generate_general_instance(&gen, n, 20) != 0 ||
                generate_super_instance(&sup, n, 5) != 0) {
                fprintf(stderr, "malloc failed\n");
                free_compare_instance(&gen);
                free_compare_instance(&sup);
                return -1;
            }
            double t0 = now_ms_compare();
            int ok_brute = solve_bruteforce(&gen);
            double t1 = now_ms_compare();
            sum_brute += (t1 - t0);
            t0 = now_ms_compare();
            int ok_super = solve_super_greedy(&sup);
            t1 = now_ms_compare();
            sum_super += (t1 - t0);
            free_compare_instance(&gen);
            free_compare_instance(&sup);
            if (!ok_brute || !ok_super) {
                fprintf(stderr, "solver failed at n=%d rep=%d\n", n, r);
                return -1;
            }
        }
        double brute_ms = sum_brute / reps;
        double super_ms = sum_super / reps;
        printf("%d,%.6f,%.6f,%d,%u\n", n, brute_ms, super_ms, reps, seed);
    }
    return 0;
}
