#include "bench.h"
#include "bitvec.h"
#include "common.h"
#include "error.h"
#include "scheme.h"

typedef struct {
    f64 keygen_ms;
    f64 encrypt_ms;
    f64 decrypt_ms;
    f64 total_ms;
} BenchSample;

static void bench_sample_add(BenchSample *sum, const BenchSample *sample) {
    sum->keygen_ms += sample->keygen_ms;
    sum->encrypt_ms += sample->encrypt_ms;
    sum->decrypt_ms += sample->decrypt_ms;
    sum->total_ms += sample->total_ms;
}
static void bench_sample_div(BenchSample *sample, u64 reps) {
    sample->keygen_ms /= (f64)reps;
    sample->encrypt_ms /= (f64)reps;
    sample->decrypt_ms /= (f64)reps;
    sample->total_ms /= (f64)reps;
}

static KnapStatus fill_message_random(BitBuf *message_bits, u64 n) {
    if (!message_bits || n == 0) {
        return KNAP_ERR_INVALID;
    }

    KnapStatus status = bit_buf_alloc(message_bits, (size_t)n);
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < n; i++) {
        message_bits->data[i] = (u8)(rand() & 1);
    }

    return KNAP_OK;
}

static f64 now_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0.0;
    }
    return (f64)ts.tv_sec * 1000.0 + (f64)ts.tv_nsec / 1000000.0;
}

static KnapStatus bench_measure_once(const SchemeOps *scheme, BitView message,
                                     const SchemeKeygenParams *params,
                                     BenchSample *out) {
    SchemeKeypair keypair = {0};
    BitBuf decrypted = {0};
    mpz_t ciphertext;
    KnapStatus status;

    f64 t0, t1;

    if (!scheme || !params || !out || !bit_view_is_valid(message)) {
        return KNAP_ERR_INVALID;
    }

    *out = (BenchSample){0};
    mpz_init(ciphertext);

    t0 = now_ms();
    status = scheme->keygen(params, &keypair);
    t1 = now_ms();
    if (status != KNAP_OK) {
        mpz_clear(ciphertext);

        return status;
    }

    out->keygen_ms = t1 - t0;

    t0 = now_ms();
    status = scheme->encrypt(&keypair, message, ciphertext);
    t1 = now_ms();
    if (status != KNAP_OK) {
        scheme->keypair_clear(&keypair);
        mpz_clear(ciphertext);

        return status;
    }

    out->encrypt_ms = t1 - t0;

    t0 = now_ms();
    status = scheme->decrypt(&keypair, ciphertext, &decrypted, 0);
    t1 = now_ms();
    if (status != KNAP_OK) {
        bit_buf_clear(&decrypted);
        scheme->keypair_clear(&keypair);
        mpz_clear(ciphertext);

        return status;
    }
    out->decrypt_ms = t1 - t0;
    out->total_ms = out->keygen_ms + out->encrypt_ms + out->decrypt_ms;

    if (!bit_view_equal(message, bit_buf_view(&decrypted))) {
        bit_buf_clear(&decrypted);
        scheme->keypair_clear(&keypair);
        mpz_clear(ciphertext);

        return KNAP_ERR_CRYPTO;
    }

    bit_buf_clear(&decrypted);
    scheme->keypair_clear(&keypair);
    mpz_clear(ciphertext);
    return KNAP_OK;
}

KnapStatus bench_run(CliFlags *flags) {
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKeygenParams params = {0};
    BenchSample sample = {0};
    BenchSample avg = {0};
    u32 seed;
    u64 reps;
    u64 warmup_reps = 3;

    if (!flags) {
        return KNAP_ERR_INVALID;
    }

    seed = flags->has_seed ? flags->seed : (u32)time(NULL);
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);

    if (flags->message_bits.length == 0) {
        if (flags->n == 0) {
            return KNAP_ERR_INVALID;
        }
        status = fill_message_random(&flags->message_bits, flags->n);
        if (status != KNAP_OK) {
            return status;
        }
    }

    reps = flags->reps > 0 ? flags->reps : 10;

    scheme = scheme_resolve(flags->scheme_id);
    params.n = flags->message_bits.length;
    params.seed = seed;
    params.flags = 0;

    printf("scheme,n,reps,warmup_reps,seed,keygen_ms,encrypt_ms,decrypt_ms,"
           "total_ms\n");

    for (u64 i = 0; i < warmup_reps; i++) {
        status = bench_measure_once(scheme, bit_buf_view(&flags->message_bits),
                                    &params, &sample);
        if (status != KNAP_OK) {
            return status;
        }
    }

    for (u64 i = 0; i < reps; i++) {
        status = bench_measure_once(scheme, bit_buf_view(&flags->message_bits),
                                    &params, &sample);
        if (status != KNAP_OK) {
            return status;
        }
        bench_sample_add(&avg, &sample);
    }
    bench_sample_div(&avg, reps);

    printf("%s,%llu,%llu,%llu,%u,%.6f,%.6f,%.6f,%.6f\n", scheme->info.id,
           (unsigned long long)flags->message_bits.length,
           (unsigned long long)reps, (unsigned long long)warmup_reps, seed,
           avg.keygen_ms, avg.encrypt_ms, avg.decrypt_ms, avg.total_ms);

    return KNAP_OK;
}
