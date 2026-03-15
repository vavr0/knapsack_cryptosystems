#include "bench.h"
#include "common.h"
#include "scheme.h"

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


KnapStatus bench_run(CliFlags *flags) {
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKeypair keypair = {0};
    SchemeKeygenParams params = {0};
    BitBuf decrypted = {0};
    mpz_t ciphertext;
    u32 seed = flags->has_seed ? flags->seed : (u32)time(NULL);
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);

    if (flags->message_bits.length == 0) {
        status = fill_message_random(&flags->message_bits, flags->n);
        if (status != KNAP_OK) {
            return status;
        }
    }

    scheme = scheme_mh_get();
    params.n = flags->message_bits.length;
    params.has_seed = flags->has_seed;
    params.seed = flags->seed;
    params.flags = 0;
    mpz_init(ciphertext);

    status = scheme->keygen(&params, &keypair);
    if (status != KNAP_OK) {
        mpz_clear(ciphertext);

        return status;
    }

    status = scheme->encrypt(&keypair, bit_buf_view(&flags->message_bits),
                             ciphertext);
    if (status != KNAP_OK) {
        scheme->keypair_clear(&keypair);
        mpz_clear(ciphertext);

        return status;
    }
    status =
        scheme->decrypt(&keypair, ciphertext, &decrypted, flags->show_steps);
    if (status != KNAP_OK) {
        scheme->keypair_clear(&keypair);
        mpz_clear(ciphertext);

        return status;
    }
    if (!bit_buf_equal(&decrypted, &flags->message_bits)) {
        bit_buf_clear(&decrypted);
        scheme->keypair_clear(&keypair);
        mpz_clear(ciphertext);

        return KNAP_ERR_CRYPTO;
    }

    bit_buf_clear(&decrypted);
    scheme->keypair_clear(&keypair);
    mpz_clear(ciphertext);
    printf("yas");

    return status;

}
