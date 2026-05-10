#include "output.h"
#include <stdio.h>
#include <stdlib.h>

void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s demo   [options]\n", prog);
    fprintf(stderr, "  %s bench  [options]\n", prog);
    fprintf(stderr, "  %s attack [options]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "Demo options:\n");
    fprintf(stderr, "  --bits BITS        Raw bitstring input, e.g. 101010\n");
    fprintf(stderr,
            "  --msg TEXT         Plaintext input; converted to bits\n");
    fprintf(
        stderr,
        "  --n BITS           Demo block size in bits; text default is 128\n");
    fprintf(stderr, "  --scheme ID        Scheme: mh-classic, mh, mh-permuted, "
                    "mh-iterated\n");
    fprintf(stderr, "  --seed SEED        Deterministic seed\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Bench options:\n");
    fprintf(stderr, "  --bits BITS        Raw bitstring input\n");
    fprintf(stderr, "  --n BITS           Random message length in bits\n");
    fprintf(stderr,
            "  --reps N           Measurement repetitions; default is 10\n");
    fprintf(stderr, "  --scheme ID        Scheme: mh-classic, mh, mh-permuted, "
                    "mh-iterated\n");
    fprintf(stderr, "  --seed SEED        Deterministic seed\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Attack options:\n");
    fprintf(stderr, "  --attack ID        Attack: brute, mitm, trapdoor-toy\n");
    fprintf(stderr, "  --bits BITS        Explicit raw bitstring input\n");
    fprintf(stderr, "  --n BITS           Random message length in bits\n");
    fprintf(stderr, "  --scheme ID        Scheme: mh-classic or mh\n");
    fprintf(stderr, "  --seed SEED        Deterministic seed\n");
}

KnapStatus print_demo_result(const CliFlags *flags, const SchemeOps *scheme,
                             const BitBlocks *blocks, const mpz_t *ciphertexts,
                             const BitBuf *decrypted_bits) {
    KnapStatus status;
    BitView used_bits;
    TextBuf decrypted_text = {0};
    char *decrypted_str = NULL;

    if (!flags || !scheme || !blocks || !ciphertexts || !decrypted_bits ||
        !decrypted_bits->data) {
        return KNAP_ERR_INVALID;
    }

    if (blocks->used_bits == 0 || blocks->used_bits > decrypted_bits->length) {
        return KNAP_ERR_INVALID;
    }

    used_bits =
        (BitView){.data = decrypted_bits->data, .length = blocks->used_bits};

    status = bit_view_to_cstr(used_bits, &decrypted_str);
    if (status != KNAP_OK) {
        return status;
    }

    if (flags->input_mode == CLI_INPUT_TEXT) {
        status = text_buf_from_bits(&decrypted_text, used_bits);
        if (status != KNAP_OK) {
            free(decrypted_str);
            return status;
        }
    }

    printf("-----BEGIN KNAPSACK MESSAGE-----\n");
    printf("Scheme: %s\n", scheme->info.id);
    printf("Block-Size: %llu\n", (unsigned long long)blocks->block_size);
    printf("Blocks: %llu\n", (unsigned long long)blocks->block_count);
    printf("\n");

    printf("Ciphertext:\n");
    for (u64 i = 0; i < blocks->block_count; i++) {
        if (i > 0) {
            printf(" ");
        }
        gmp_printf("%Zd", ciphertexts[i]);
    }
    printf("\n\n");

    if (flags->input_mode == CLI_INPUT_TEXT) {
        printf("Decrypted-Text:\n");
        printf("%s\n\n", decrypted_text.data);
    }

    printf("Decrypted-Bits:\n");
    printf("%s\n\n", decrypted_str);

    printf("Status: OK\n");
    printf("-----END KNAPSACK MESSAGE-----\n");

    text_buf_clear(&decrypted_text);
    free(decrypted_str);
    return KNAP_OK;
}

void print_attack_result(const char *attack_id, const SchemeOps *scheme, u64 n,
                         const u64 seed[2], const AttackResult *result,
                         f64 attack_ms) {
    printf("attack,scheme,n,initstate,initseq,success,attack_ms,checked,table_"
           "entries\n");
    printf("%s,%s,%llu,%llu,%llu,%d,%.6f,%llu,%llu\n", attack_id,
           scheme->info.id, (unsigned long long)n, (unsigned long long)seed[0],
           (unsigned long long)seed[1], (int)result->success, attack_ms,
           (unsigned long long)result->checked_count,
           (unsigned long long)result->table_entries);
}
