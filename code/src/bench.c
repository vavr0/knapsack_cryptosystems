#include "bench.h"
#include "buffer.h"
#include "common.h"
#include "error.h"
#include "rand.h"
#include "scheme.h"
#include "scheme_mh_common.h"
#include "seed.h"
#include "utils.h"
#include <stdio.h>

typedef struct {
    f64 keygen_ms;
    f64 encrypt_ms;
    f64 decrypt_ms;
    f64 total_ms;
} BenchSample;

typedef struct {
    u64 delta_max;
    u64 margin_bound;
    u64 margin;
    u64 sum_bits;
    f64 density;
} BenchKeyStats;

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

static KnapStatus bench_key_stats_from_mh(const MhKey *key,
                                          BenchKeyStats *out) {
    mpz_t sum;
    mpz_t margin;
    u64 max_public_bits = 0;

    if (!key || !out || key->n == 0) {
        return KNAP_ERR_INVALID;
    }

    *out = (BenchKeyStats){0};
    mpz_inits(sum, margin, NULL);

    for (u64 i = 0; i < key->n; i++) {
        u64 bits;

        mpz_add(sum, sum, key->priv_weights[i]);

        bits = (u64)mpz_sizeinbase(key->pub_weights[i], 2);
        if (bits > max_public_bits) {
            max_public_bits = bits;
        }
    }

    mpz_sub(margin, key->mod, sum);

    out->delta_max = MH_DEFAULT_DELTA_MAX;
    out->margin_bound = MH_DEFAULT_MARGIN_FACTOR * key->n;
    out->margin = mpz_get_ui(margin);
    out->sum_bits = (u64)mpz_sizeinbase(sum, 2);
    out->density = (f64)key->n / (f64)max_public_bits;

    mpz_clears(sum, margin, NULL);
    return KNAP_OK;
}

static KnapStatus bench_measure_once(const SchemeOps *scheme, BitView message,
                                     const SchemeKeygenParams *params,
                                     BenchSample *out) {
    SchemeKey keypair = {0};
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
        scheme->scheme_key_clear(&keypair);
        mpz_clear(ciphertext);

        return status;
    }

    out->encrypt_ms = t1 - t0;

    t0 = now_ms();
    status = scheme->decrypt(&keypair, ciphertext, &decrypted);
    t1 = now_ms();
    if (status != KNAP_OK) {
        bit_buf_clear(&decrypted);
        scheme->scheme_key_clear(&keypair);
        mpz_clear(ciphertext);

        return status;
    }
    out->decrypt_ms = t1 - t0;
    out->total_ms = out->keygen_ms + out->encrypt_ms + out->decrypt_ms;

    if (!bit_view_equal(message, bit_buf_view(&decrypted))) {
        bit_buf_clear(&decrypted);
        scheme->scheme_key_clear(&keypair);
        mpz_clear(ciphertext);

        return KNAP_ERR_CRYPTO;
    }

    bit_buf_clear(&decrypted);
    scheme->scheme_key_clear(&keypair);
    mpz_clear(ciphertext);
    return KNAP_OK;
}

KnapStatus bench_run(CliFlags *flags) {
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKeygenParams params = {0};
    BenchSample sample = {0};
    BenchSample avg = {0};
    u64 reps;
    u64 warmup_reps = 3;
    PrngState rng = {0};
    u64 seed[2];

    if (!flags) {
        return KNAP_ERR_INVALID;
    }

    status = seed_resolve_pair(flags->has_seed, flags->seed, seed);
    if (status != KNAP_OK) {
        return status;
    }
    prng_seed(&rng, seed[0], seed[1]);

    if (flags->bits_message.length == 0) {
        if (flags->n == 0) {
            return KNAP_ERR_INVALID;
        }
        status = fill_message_random(&flags->bits_message, flags->n, &rng);
        if (status != KNAP_OK) {
            return status;
        }
    }

    reps = flags->reps > 0 ? flags->reps : 10;

    scheme = scheme_resolve(flags->scheme_id);
    if (!scheme) {
        return KNAP_ERR_INVALID;
    }
    params.n = flags->bits_message.length;
    params.initstate = seed[0];
    params.initseq = seed[1];
    params.flags = 0;

    BenchKeyStats key_stats = {0};
    SchemeKey stats_key = {0};

    status = scheme->keygen(&params, &stats_key);
    if (status != KNAP_OK) {
        return status;
    }

    status = bench_key_stats_from_mh((const MhKey *)stats_key.data, &key_stats);
    scheme->scheme_key_clear(&stats_key);
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < warmup_reps; i++) {
        status = bench_measure_once(scheme, bit_buf_view(&flags->bits_message),
                                    &params, &sample);
        if (status != KNAP_OK) {
            return status;
        }
    }

    for (u64 i = 0; i < reps; i++) {
        status = bench_measure_once(scheme, bit_buf_view(&flags->bits_message),
                                    &params, &sample);
        if (status != KNAP_OK) {
            return status;
        }
        bench_sample_add(&avg, &sample);
    }
    bench_sample_div(&avg, reps);

    printf("scheme,n,reps,warmup_reps,initstate,initseq,keygen_ms,encrypt_ms,"
           "decrypt_ms,total_ms,delta_max,margin_bound,margin,sum_bits,"
           "density\n");

    printf("%s,%llu,%llu,%llu,%lu,%lu,%.6f,%.6f,%.6f,%.6f,%llu,%llu,%llu,"
           "%llu,%.6f\n",
           scheme->info.id, (unsigned long long)flags->bits_message.length,
           (unsigned long long)reps, (unsigned long long)warmup_reps, seed[0],
           seed[1], avg.keygen_ms, avg.encrypt_ms, avg.decrypt_ms, avg.total_ms,
           (unsigned long long)key_stats.delta_max,
           (unsigned long long)key_stats.margin_bound,
           (unsigned long long)key_stats.margin,
           (unsigned long long)key_stats.sum_bits, key_stats.density);

    return KNAP_OK;
}
