#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

/* ============================================================
   Simple experiment program for knapsack problem

   - General knapsack solved by brute force (O(2^n))
   - Superincreasing knapsack solved by greedy (O(n))
   - Measures running time and prints results for comparison

   This is standalone; it does NOT depend on cryptosystem code.
   ============================================================ */

typedef struct {
    int n;          // number of items
    int *w;         // weights
    int target;     // target sum
} KnapsackInstance;

/* ---------- Utility: timing ---------- */

static double now_ms(void) {
    return 1000.0 * (double)clock() / (double)CLOCKS_PER_SEC;
}

/* ---------- Instance generators ---------- */

/* Generate a random general knapsack instance.
   Weights are in [1, maxW]. Target is sum of a random subset. */
void generate_general_instance(KnapsackInstance *inst, int n, int maxW) {
    inst->n = n;
    inst->w = malloc(n * sizeof(int));
    if (!inst->w) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    int i;
    for (i = 0; i < n; i++) {
        inst->w[i] = 1 + rand() % maxW;
    }

    // Choose random subset and compute target
    int target = 0;
    for (i = 0; i < n; i++) {
        int bit = rand() & 1;
        if (bit) target += inst->w[i];
    }
    inst->target = target;
}

/* Generate a random superincreasing instance.
   w[0] >= 1 and w[i] > sum_{j < i} w[j].
   Target is sum of random subset. */
void generate_superincreasing_instance(KnapsackInstance *inst, int n, int baseStep) {
    inst->n = n;
    inst->w = malloc(n * sizeof(int));
    if (!inst->w) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    int i;
    int sum = 0;
    for (i = 0; i < n; i++) {
        int increment = 1 + rand() % baseStep;  // small random extra
        inst->w[i] = sum + increment;           // ensures superincreasing
        sum += inst->w[i];
    }

    // Choose random subset and compute target
    int target = 0;
    for (i = 0; i < n; i++) {
        int bit = rand() & 1;
        if (bit) target += inst->w[i];
    }
    inst->target = target;
}

void free_instance(KnapsackInstance *inst) {
    free(inst->w);
    inst->w = NULL;
    inst->n = 0;
    inst->target = 0;
}

/* ---------- Solvers ---------- */

/* Brute-force knapsack solver for general instance.
   Returns true if it finds a solution equal to target.
   If solution_out != NULL, fills it with 0/1 bits (length n). */
bool knapsack_bruteforce(const KnapsackInstance *inst, int *solution_out) {
    int n = inst->n;
    int target = inst->target;
    int64_t total_subsets = 1LL << n;  // careful: n should be <= 30

    for (int64_t mask = 0; mask < total_subsets; mask++) {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1LL << i)) {
                sum += inst->w[i];
            }
        }
        if (sum == target) {
            if (solution_out) {
                for (int i = 0; i < n; i++) {
                    solution_out[i] = (mask & (1LL << i)) ? 1 : 0;
                }
            }
            return true;
        }
    }
    return false;
}

/* Greedy solver for superincreasing instance.
   Weights must satisfy w[i] > sum_{j < i} w[j].
   Unique solution if it exists.
   Returns true if solution exists and matches target. */
bool knapsack_super_greedy(const KnapsackInstance *inst, int *solution_out) {
    int n = inst->n;
    int s = inst->target;

    // Start from largest weight
    for (int i = n - 1; i >= 0; i--) {
        if (inst->w[i] <= s) {
            solution_out[i] = 1;
            s -= inst->w[i];
        } else {
            solution_out[i] = 0;
        }
    }
    return (s == 0);
}

/* ---------- Benchmark helpers ---------- */

double benchmark_bruteforce(int n, int maxW, int repetitions) {
    double start, end;
    double total_ms = 0.0;

    for (int r = 0; r < repetitions; r++) {
        KnapsackInstance inst;
        generate_general_instance(&inst, n, maxW);

        int *solution = malloc(n * sizeof(int));
        if (!solution) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        start = now_ms();
        (void)knapsack_bruteforce(&inst, solution);
        end = now_ms();

        total_ms += (end - start);

        free(solution);
        free_instance(&inst);
    }

    return total_ms / repetitions;
}

double benchmark_super_greedy(int n, int baseStep, int repetitions) {
    double start, end;
    double total_ms = 0.0;

    for (int r = 0; r < repetitions; r++) {
        KnapsackInstance inst;
        generate_superincreasing_instance(&inst, n, baseStep);

        int *solution = malloc(n * sizeof(int));
        if (!solution) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        start = now_ms();
        (void)knapsack_super_greedy(&inst, solution);
        end = now_ms();

        total_ms += (end - start);

        free(solution);
        free_instance(&inst);
    }

    return total_ms / repetitions;
}

/* ---------- Pretty printing ---------- */

void print_instance(const KnapsackInstance *inst) {
    printf("n = %d\n", inst->n);
    printf("weights: [");
    for (int i = 0; i < inst->n; i++) {
        printf("%d", inst->w[i]);
        if (i + 1 < inst->n) printf(", ");
    }
    printf("]\n");
    printf("target = %d\n", inst->target);
}

void print_solution(const int *sol, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", sol[i]);
        if (i + 1 < n) printf(" ");
    }
    printf("]\n");
}

/* ---------- Main interactive menu ---------- */

void menu_single_experiment(void) {
    int n;
    printf("Enter number of items n (<= 25 recommended): ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > 25) {
        printf("Invalid n.\n");
        return;
    }

    printf("\n--- General knapsack instance (brute force) ---\n");
    KnapsackInstance gen;
    generate_general_instance(&gen, n, 20);
    print_instance(&gen);

    int *sol_gen = malloc(n * sizeof(int));
    double t_start = now_ms();
    bool ok_gen = knapsack_bruteforce(&gen, sol_gen);
    double t_end = now_ms();

    if (ok_gen) {
        printf("Solution found (brute force):\n");
        print_solution(sol_gen, n);
    } else {
        printf("No solution found (this can happen, but target is constructed to be solvable,\n"
               "so this would be surprising).\n");
    }
    printf("Time (brute force): %.3f ms\n", t_end - t_start);

    free(sol_gen);
    free_instance(&gen);

    printf("\n--- Superincreasing knapsack instance (greedy) ---\n");
    KnapsackInstance sup;
    generate_superincreasing_instance(&sup, n, 5);
    print_instance(&sup);

    int *sol_sup = malloc(n * sizeof(int));
    t_start = now_ms();
    bool ok_sup = knapsack_super_greedy(&sup, sol_sup);
    t_end = now_ms();

    if (ok_sup) {
        printf("Solution found (greedy):\n");
        print_solution(sol_sup, n);
    } else {
        printf("No solution (greedy failed, but for superincreasing it should always work\n"
               "if target was built as a subset sum).\n");
    }
    printf("Time (greedy): %.3f ms\n", t_end - t_start);

    free(sol_sup);
    free_instance(&sup);
}

void menu_batch_experiment(void) {
    int n_min, n_max;
    printf("Enter n_min and n_max (e.g. 10 24, with n_max <= 25): ");
    if (scanf("%d %d", &n_min, &n_max) != 2 || n_min <= 0 || n_max < n_min || n_max > 25) {
        printf("Invalid range.\n");
        return;
    }

    int repetitions = 3; // average over a few runs

    printf("\n# n, brute_force_ms, super_greedy_ms\n");
    for (int n = n_min; n <= n_max; n++) {
        double t_brute = benchmark_bruteforce(n, 20, repetitions);
        double t_greedy = benchmark_super_greedy(n, 5, repetitions);
        printf("%d, %.6f, %.6f\n", n, t_brute, t_greedy);
        fflush(stdout);
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    while (1) {
        printf("\n===============================\n");
        printf(" Knapsack Experiment Menu\n");
        printf("===============================\n");
        printf("1) Single experiment (show instances + solutions)\n");
        printf("2) Batch experiment (print table for graphing)\n");
        printf("0) Quit\n");
        printf("Choice: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Input error, exiting.\n");
            break;
        }

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            menu_single_experiment();
        } else if (choice == 2) {
            menu_batch_experiment();
        } else {
            printf("Unknown option.\n");
        }
    }

    return 0;
}
