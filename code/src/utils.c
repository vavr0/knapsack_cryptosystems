#include "utils.h"
#include "buffer.h"
#include "error.h"
#include "rand.h"

f64 now_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0.0;
    }
    return (f64)ts.tv_sec * 1000.0 + (f64)ts.tv_nsec / 1000000.0;
}

KnapStatus fill_message_random(BitBuf *bits_message, u64 n, PrngState *rng) {
    if (!bits_message || n == 0) {
        return KNAP_ERR_INVALID;
    }

    KnapStatus status = bit_buf_alloc(bits_message, (size_t)n);
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < n; i++) {
        bits_message->data[i] = (u8)(prng_rand(rng) & 1);
    }

    return KNAP_OK;
}
