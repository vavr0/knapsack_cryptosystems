#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double now_ms(void) {
    return 1000.0 * (double)clock() / (double)CLOCKS_PER_SEC;
}

int knapsack_run_once(const KnapsackRunRequest *req, KnapsackRunOutput *out,
                      KnapsackRunMetrics *metrics) {
    if (!req || !out || !req->message_bits || req->n == 0) {
        return -1;
    }

    const SchemeOps *ops = req->scheme ? req->scheme : scheme_mh_get();

    memset(out, 0, sizeof(*out));
    out->n = req->n;

    if (metrics) {
        metrics->keygen_ms = 0.0;
        metrics->encrypt_ms = 0.0;
        metrics->decrypt_ms = 0.0;
        metrics->total_ms = 0.0;
    }

    int *decrypted = malloc(req->n * sizeof(int));
    if (!decrypted) {
        return -1;
    }

    SchemeKeypair keypair = {0};
    mpz_t ciphertext;
    double t0, t1;

    mpz_init(ciphertext);

    t0 = now_ms();
    if (ops->keygen(req->n, &keypair) != 0) {
        mpz_clear(ciphertext);
        free(decrypted);
        return -1;
    }
    t1 = now_ms();
    if (metrics) {
        metrics->keygen_ms = t1 - t0;
    }

    t0 = now_ms();
    if (ops->encrypt(&keypair, req->message_bits, req->n, ciphertext) != 0) {
        mpz_clear(ciphertext);
        ops->keypair_clear(&keypair);
        free(decrypted);
        return -1;
    }
    t1 = now_ms();
    if (metrics) {
        metrics->encrypt_ms = t1 - t0;
    }

    t0 = now_ms();
    if (ops->decrypt(&keypair, ciphertext, decrypted, req->n, req->show_steps != 0) != 0) {
        mpz_clear(ciphertext);
        ops->keypair_clear(&keypair);
        free(decrypted);
        return -1;
    }
    t1 = now_ms();
    if (metrics) {
        metrics->decrypt_ms = t1 - t0;
        metrics->total_ms = metrics->keygen_ms + metrics->encrypt_ms + metrics->decrypt_ms;
    }

    for (size_t i = 0; i < req->n; i++) {
        if (decrypted[i] != req->message_bits[i]) {
            ops->keypair_clear(&keypair);
            mpz_clear(ciphertext);
            free(decrypted);
            return -1;}
    }

    out->decrypted_bits = decrypted;

    if (req->capture_details) {
        out->keypair = keypair;
        out->scheme = ops;
        out->has_key = 1;
        mpz_init_set(out->ciphertext, ciphertext);
        out->has_ciphertext = 1;
        mpz_clear(ciphertext);
    } else {
        ops->keypair_clear(&keypair);
        mpz_clear(ciphertext);
    }

    return 0;
}

void knapsack_run_output_clear(KnapsackRunOutput *out) {
    if (!out) return;
    free(out->decrypted_bits);
    out->decrypted_bits = NULL;
    out->n = 0;
    if (out->has_ciphertext) {
        mpz_clear(out->ciphertext);
        out->has_ciphertext = 0;
    }
    if (out->has_key) {
        if (out->scheme && out->scheme->keypair_clear) {
            out->scheme->keypair_clear(&out->keypair);
        }
        out->has_key = 0;
    }
    out->scheme = NULL;
}
