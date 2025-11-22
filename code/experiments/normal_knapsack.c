#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void generate_random_weights(int *weights, int num_weights, int max_value) {
    for (int i = 0; i < num_weights; i++) {
        weights[i] = 1 + rand() % max_value;
    }
}

int generate_random_target(const int *weights, int num_weights, int *chosen_bits) {
    int target_sum = 0;
    for (int i = 0; i < num_weights; i++) {
        chosen_bits[i] = rand() % 2;
        if (chosen_bits[i])
            target_sum += weights[i];
    }
    return target_sum;
}

int brute_force_solve(const int *weights, int num_weights, int target_sum, int *solution_bits) {
    long long total_combinations = 1LL << num_weights;

    for (long long mask = 0; mask < total_combinations; mask++) {
        int sum = 0;
        for (int i = 0; i < num_weights; i++) {
            if (mask & (1LL << i))
                sum += weights[i];
        }

        if (sum == target_sum) { // found solution
            for (int i = 0; i < num_weights; i++)
                solution_bits[i] = (mask & (1LL << i)) ? 1 : 0;
            return 1;
        }
    }

    return 0; // no solution found
}

int main(int argc, char **argv) {
    int num_weights = (argc > 1) ? atoi(argv[1]) : 8;
    if (num_weights <= 0 || num_weights > 30) {
        num_weights = 8;
    }

    srand((unsigned)time(NULL));

    int weights[64] = {0};
    int chosen_bits[64] = {0};
    int recovered_bits[64] = {0};

    generate_random_weights(weights, num_weights, 20);

    printf("Normal random weights:\n");
    for (int i = 0; i < num_weights; i++)
        printf("w[%d] = %d\n", i, weights[i]);

    int target_sum = generate_random_target(weights, num_weights, chosen_bits);

    printf("\nChosen bits: ");
    for (int i = 0; i < num_weights; i++)
        printf("%d", chosen_bits[i]);
    printf("\nTarget sum: %d\n", target_sum);

    clock_t start = clock();
    int found = brute_force_solve(weights, num_weights, target_sum, recovered_bits);
    clock_t end = clock();

    if (found) {
        printf("Recovered bits: ");
        for (int i = 0; i < num_weights; i++)
            printf("%d", recovered_bits[i]);
        printf("\n");
    } else {
        printf("No subset found.\n");
    }

    printf("Time: %.6f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}
