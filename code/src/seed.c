#include "seed.h"
#include "common.h"
#include "error.h"

#if defined(_WIN32)
#include <bcrypt.h>
#include <windows.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||    \
    defined(__NetBSD__)
#include <stdlib.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#if defined(__linux__)
#include <sys/random.h>
#endif
#endif

static u64 mix_u64(u64 x) {
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;
    return x;
}

void seed_expand_u64(u64 seed, u64 out[2]) {
    if (!out) {
        return;
    }

    out[0] = mix_u64(seed ^ 0x9e3779b97f4a7c15ULL);
    out[1] = mix_u64(seed ^ 0xd1b54a32d192ed03ULL);
}

KnapStatus seed_get_entropy(void *buf, size_t len) {
    if (!buf && len > 0) {
        return KNAP_ERR_INVALID;
    }
    if (len == 0) {
        return KNAP_OK;
    }

#if defined(_WIN32)
    NTSTATUS rc =
        BCryptGenRandom(NULL, buf, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

    if (rc != STATUS_SUCCESS) {
        return KNAP_ERR_INTERNAL;
    }
    return KNAP_OK;
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||    \
    defined(__NetBSD__)
    arc4random_buf(buf, len);
    return KNAP_OK;
#elif defined(__linux__)
    size_t filled = 0;
    while (filled < len) {
        ssize_t n = getrandom((u8 *)buf + filled, len - filled, 0);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        filled += (size_t)n;
    }
    if (filled == len) {
        return KNAP_OK;
    }
    return KNAP_ERR_INTERNAL;
#else
    return KNAP_ERR_INTERNAL;
#endif
}

KnapStatus seed_resolve_pair(b8 has_seed, u64 seed, u64 out[2]) {
    if (out == NULL) {
        return KNAP_ERR_INVALID;
    }

    if (has_seed) {
        seed_expand_u64(seed, out);
        return KNAP_OK;
    }

    KnapStatus status = seed_get_entropy(out, sizeof(u64) * 2);
    return status;
}
