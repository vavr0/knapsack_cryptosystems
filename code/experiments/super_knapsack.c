#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void generate_superincreasing(int *weights, int num_weights) {
    weights[0] = 1 + rand() % 5;
    for (int i = 1; i < num_weights; i++) {
        weights[i] = weights[i - 1] * 2 + (rand() % 3);
    }
}

int generate_random_target(const int *weights, int n, int *chosen_bits) {
    int target = 0;
    for (int i = 0; i < n; i++) {
        if (rand() % 2) {
            chosen_bits[i] = 1;
            target += weights[i];
        } else {
            chosen_bits[i] = 0;
        }
    }
    return target;
}

static void greedy_solve(const int *weights, int num_weights, int target, int *solution_bits) {
    for (int i = num_weights - 1; i >= 0; i--) {
        if (target >= weights[i]) {
            solution_bits[i] = 1;
            target -= weights[i];
        } else
            solution_bits[i] = 0;
    }
}

int main(int argc, char **argv) {
    int num_weights = (argc > 1) ? atoi(argv[1]) : 8;
    if (num_weights <= 0 || num_weights > 30) {
        num_weights = 8;
    }

    srand((unsigned)time(NULL));
    int weights[64] = {0}; 
    int solution[64] = {0}; 
    int chosen_bits[64] = {0}; 
    int sum = 0;
    int target = 0;

    generate_superincreasing(weights, num_weights);
    for (int i = 0; i < num_weights; i++) {
        sum += weights[i];
    }

    target = generate_random_target(weights, num_weights, chosen_bits);
    printf("Random bits chosen: ");
    for (int i = 0; i < num_weights; i++) printf("%d", chosen_bits[i]);
    printf("\nTarget sum: %d\n", target);

    printf("Superincreasing sequence (n=%d):\n", num_weights);
    for (int i = 0; i < num_weights; i++)
        printf("w[%d]=%d\n", i, weights[i]);
    printf("Target=%d (<= sum=%d)\n", target, sum);

    greedy_solve(weights, num_weights, target, solution);

    printf("Recovered bits: ");
    for (int i = 0; i < num_weights; i++)
        printf("%d", solution[i]);
    printf("\n");
    return 0;
}
