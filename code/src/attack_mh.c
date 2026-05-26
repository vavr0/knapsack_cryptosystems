#include "attack_mh.h"
#include "error.h"
#include "output.h"
#include "scheme.h"
#include "scheme_mh_common.h"
#include "seed.h"
#include "utils.h"
#include <gmp.h>
#include <stdlib.h>
#include <string.h>

static void subset_sum(const mpz_t *weights, u64 offset, u64 count, u64 mask,
                       mpz_t out_sum) {
    mpz_set_ui(out_sum, 0);

    for (u64 i = 0; i < count; i++) {
        if ((mask >> i) & 1u) {
            mpz_add(out_sum, out_sum, weights[offset + i]);
        }
    }
}

// Brute-force subset-sum search over all bit masks.
static KnapStatus attack_brute_force(MhPublicKeyView pub_key,
                                     const mpz_t ciphertext,
                                     AttackResult *result) {
    if (!pub_key.weights || !result || pub_key.n == 0 ||
        pub_key.n > ATTACK_BRUTE_MAX_N) {
        return KNAP_ERR_INVALID;
    }

    KnapStatus status;
    mpz_t sum;

    u64 limit = 1ULL << pub_key.n;

    mpz_init(sum);

    for (u64 mask = 0; mask < limit; mask++) {
        subset_sum(pub_key.weights, 0, pub_key.n, mask, sum);

        result->checked_count++;

        if (mpz_cmp(sum, ciphertext) == 0) {
            status = bit_buf_alloc(&result->message, (size_t)pub_key.n);
            if (status != KNAP_OK) {
                mpz_clear(sum);
                return status;
            }

            for (u64 i = 0; i < pub_key.n; i++) {
                result->message.data[i] = (u8)((mask >> i) & 1u);
            }

            result->success = 1;
            mpz_clear(sum);
            return KNAP_OK;
        }
    }

    mpz_clear(sum);
    return KNAP_OK;
}

static int mitm_entry_cmp(const void *a, const void *b) {
    const MitmEntry *entry_a = (const MitmEntry *)a;
    const MitmEntry *entry_b = (const MitmEntry *)b;

    return mpz_cmp(entry_a->value, entry_b->value);
}

static void mitm_entries_clear(MitmEntry *entries, u64 count) {
    if (!entries) {
        return;
    }

    for (u64 i = 0; i < count; i++) {
        mpz_clear(entries[i].value);
    }

    free(entries);
}

static KnapStatus mitm_build_message(u64 left_mask, u64 left_n, u64 right_mask,
                                     u64 right_n, BitBuf *out_message) {
    KnapStatus status = bit_buf_alloc(out_message, (size_t)(left_n + right_n));
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < left_n; i++) {
        out_message->data[i] = (u8)((left_mask >> i) & 1u);
    }

    for (u64 i = 0; i < right_n; i++) {
        out_message->data[left_n + i] = (u8)((right_mask >> i) & 1u);
    }

    return KNAP_OK;
}

// Meet-in-the-middle search using left sums and C - right sums.
static KnapStatus attack_meet_in_middle(MhPublicKeyView pub_key,
                                        const mpz_t ciphertext,
                                        AttackResult *result) {
    if (!pub_key.weights || !result || pub_key.n == 0) {
        return KNAP_ERR_INVALID;
    }

    u64 left_n = pub_key.n / 2;
    u64 right_n = pub_key.n - left_n;

    if (left_n > ATTACK_MITM_MAX_HALF_N || right_n > ATTACK_MITM_MAX_HALF_N) {
        return KNAP_ERR_INVALID;
    }

    u64 left_count = 1ULL << left_n;
    u64 right_count = 1ULL << right_n;

    result->table_entries = left_count + right_count;

    MitmEntry *left_entries = NULL;
    MitmEntry *right_entries = NULL;
    mpz_t sum;
    KnapStatus status;

    left_entries = malloc((size_t)left_count * sizeof(*left_entries));
    if (!left_entries) {
        return KNAP_ERR_ALLOC;
    }

    right_entries = malloc((size_t)right_count * sizeof(*right_entries));
    if (!right_entries) {
        free(left_entries);
        return KNAP_ERR_ALLOC;
    }

    mpz_init(sum);

    for (u64 mask = 0; mask < left_count; mask++) {
        mpz_init(left_entries[mask].value);
        left_entries[mask].mask = mask;
        subset_sum(pub_key.weights, 0, left_n, mask, left_entries[mask].value);
    }

    for (u64 mask = 0; mask < right_count; mask++) {
        mpz_init(right_entries[mask].value);
        right_entries[mask].mask = mask;
        subset_sum(pub_key.weights, left_n, right_n, mask, sum);
        mpz_sub(right_entries[mask].value, ciphertext, sum);
    }
    mpz_clear(sum);

    qsort(left_entries, (size_t)left_count, sizeof(*left_entries),
          mitm_entry_cmp);

    qsort(right_entries, (size_t)right_count, sizeof(*right_entries),
          mitm_entry_cmp);

    u64 left_i = 0;
    u64 right_i = 0;

    while (left_i < left_count && right_i < right_count) {
        int cmp =
            mpz_cmp(left_entries[left_i].value, right_entries[right_i].value);

        result->checked_count++;

        if (cmp == 0) {
            status = mitm_build_message(left_entries[left_i].mask, left_n,
                                        right_entries[right_i].mask, right_n,
                                        &result->message);
            if (status != KNAP_OK) {
                mitm_entries_clear(left_entries, left_count);
                mitm_entries_clear(right_entries, right_count);
                return status;
            }

            result->success = 1;
            break;
        }

        if (cmp < 0) {
            left_i++;
        } else {
            right_i++;
        }
    }

    mitm_entries_clear(left_entries, left_count);
    mitm_entries_clear(right_entries, right_count);

    return KNAP_OK;
}

KnapStatus attack_mh_run(CliFlags *flags) {
    AttackResult result = {0};
    KnapStatus status;
    const SchemeOps *scheme;
    SchemeKey scheme_key = {0};
    SchemeKeygenParams params = {0};
    MhKey *key;
    MhPublicKeyView pub_key;
    PrngState rng = {0};
    u64 seed[2];
    mpz_t ciphertext;
    f64 t0;
    f64 t1;
    f64 attack_ms;

    if (!flags || !flags->attack_id) {
        return KNAP_ERR_INVALID;
    }

    if (strcmp(flags->attack_id, "brute") != 0 &&
        strcmp(flags->attack_id, "mitm") != 0) {
        return KNAP_ERR_INVALID;
    }

    status = seed_resolve_pair(flags->has_seed, flags->seed, seed);
    if (status != KNAP_OK) {
        return status;
    }

    prng_seed(&rng, seed[0], seed[1]);

    if (flags->bits_message.length == 0) {
        status = fill_message_random(&flags->bits_message, flags->n, &rng);
        if (status != KNAP_OK) {
            return status;
        }
    }
    scheme = scheme_resolve(flags->scheme_id);
    if (!scheme) {
        return KNAP_ERR_INVALID;
    }

    params.n = flags->bits_message.length;
    params.initstate = seed[0];
    params.initseq = seed[1];
    params.flags = 0;

    status = scheme->keygen(&params, &scheme_key);
    if (status != KNAP_OK) {
        return status;
    }

    mpz_init(ciphertext);

    status = scheme->encrypt(&scheme_key, bit_buf_view(&flags->bits_message),
                             ciphertext);
    if (status != KNAP_OK) {
        mpz_clear(ciphertext);
        scheme->scheme_key_clear(&scheme_key);
        return status;
    }

    key = (MhKey *)scheme_key.data;
    pub_key = (MhPublicKeyView){
        .n = key->n,
        .weights = (const mpz_t *)key->pub_weights,
    };

    t0 = now_ms();
    if (strcmp(flags->attack_id, "brute") == 0) {
        status = attack_brute_force(pub_key, ciphertext, &result);
    } else if (strcmp(flags->attack_id, "mitm") == 0) {
        status = attack_meet_in_middle(pub_key, ciphertext, &result);
    } else {
        status = KNAP_ERR_INVALID;
    }
    t1 = now_ms();
    attack_ms = t1 - t0;

    if (status != KNAP_OK) {
        mpz_clear(ciphertext);
        attack_result_clear(&result);
        scheme->scheme_key_clear(&scheme_key);
        return status;
    }

    if (result.success &&
        !bit_buf_equal(&flags->bits_message, &result.message)) {
        result.success = 0;
    }

    print_attack_result(flags->attack_id, scheme, flags->bits_message.length,
                        seed, &result, attack_ms);

    mpz_clear(ciphertext);
    attack_result_clear(&result);
    scheme->scheme_key_clear(&scheme_key);

    return KNAP_OK;
}

void attack_result_clear(AttackResult *result) {
    if (!result) {
        return;
    }

    bit_buf_clear(&result->message);
    *result = (AttackResult){0};
}
