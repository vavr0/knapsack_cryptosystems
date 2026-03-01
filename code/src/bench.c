#include "bench.h"
#include "common.h"
#include "scheme.h"
#include "system.h"
#include "utils.h"

static void fill_message_random(i32 *message, i32 n) {
    for (i32 i = 0; i < n; i++) {
        message[i] = rand() & 1;
    }
}

i32 bench_run_pipeline_csv(i32 n_min, i32 n_max, i32 reps, u32 seed,
                           const char *message_bits) {
    (void)seed;
    if (n_min <= 0 || n_max < n_min || reps <= 0) {
        return -1;
    }

    for (i32 n = n_min; n <= n_max; n++) {
        f64 sum_keygen = 0.0;
        f64 sum_encrypt = 0.0;
        f64 sum_decrypt = 0.0;

        i32 *message = malloc((size_t)n * sizeof(i32));
        if (!message) {
            fprintf(stderr, "malloc failed\n");
            free(message);
            return -1;
        }

        if (message_bits) {
            if (bits_to_array(message_bits, message, (size_t)n) != 0) {
                fprintf(stderr, "filling msg failed\n");
                free(message);
                return -1;
            }
        } else {
            fill_message_random(message, n);
        }

        for (i32 r = 0; r < reps; r++) {
            KnapsackRunRequest req = {0};
            KnapsackRunOutput out = {0};
            KnapsackRunMetrics metrics = {0};

            req.n = (size_t)n;
            req.scheme = scheme_mh_get();
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
        }
        free(message);
        f64 keygen_ms = sum_keygen / reps;
        f64 encrypt_ms = sum_encrypt / reps;
        f64 decrypt_ms = sum_decrypt / reps;
        f64 total_ms = keygen_ms + encrypt_ms + decrypt_ms;
        printf("%d,%.6f,%.6f,%.6f,%.6f,%d,%u\n", n, keygen_ms, encrypt_ms,
               decrypt_ms, total_ms, reps, seed);
    }
    return 0;
}
