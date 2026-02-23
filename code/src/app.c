#include "keygen.h"
#include "encrypt.h"
#include "decrypt.h"
#include "utils.h"
#include "app.h"
#include <string.h>
#include <stdbool.h>

#define MAX_MESSAGE_BITS 128



static size_t read_message_bits(int **message_out) {
    char line[256];

    printf("Enter plaintext bits (e.g. 101010): ");
    if (!fgets(line, sizeof(line), stdin)) {
        return 0;
    }

    size_t len = strcspn(line, "\r\n");
    line[len] = '\0';

    if (len == 0 || len > MAX_MESSAGE_BITS) {
        fprintf(stderr, "Invalid length. Use 1..%d bits.\n", MAX_MESSAGE_BITS);
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        if (line[i] != '0' && line[i] != '1') {
            fprintf(stderr, "Invalid bit '%c'. Use only 0 or 1.\n", line[i]);
            return 0;
        }
    }

    int *message = malloc(len * sizeof(int));
    if (!message) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        message[i] = line[i] - '0';
    }

    *message_out = message;
    return len;
}

int run_bench(int argc, char **argv) {
    // TODO
    (void)argc;
    (void)argv;
    return 1;
}

int run_demo(int argc, char **argv) {
    (void)argc;
    (void)argv;
    printf("===knapsack demo===\n");

    int *message = NULL;
    size_t n = read_message_bits(&message);
    if (n == 0) {
        return 1;
    }

    KnapsackKey key;
    keygen_init(&key, n);
    keygen_generate(&key);

    printf("Private key (superincreasing):\n");
    for (size_t i = 0; i < key.n; i++)
        gmp_printf("w[%zu] = %Zd\n", i, key.w[i]);

    printf("\nPublic key:\n");
    for (size_t i = 0; i < key.n; i++)
        gmp_printf("b[%zu] = %Zd\n", i, key.b[i]);

    gmp_printf("\nm = %Zd\nn = %Zd\n", key.m, key.n_mult);

    printf("\nPlaintext bits: ");
    for (size_t i = 0; i < n; i++)
        printf("%d", message[i]);
    printf("\n");

    // Step 3: Encrypt
    mpz_t ciphertext;
    mpz_init(ciphertext);
    encrypt_message(&key, message, ciphertext);

    printf("\nCiphertext:\n");
    gmp_printf("C = %Zd\n", ciphertext);


    int *decrypted = malloc(n * sizeof(int));
    if (!decrypted) {
        fprintf(stderr, "Memory allocation failed.\n");
        mpz_clear(ciphertext);
        keygen_clear(&key);
        free(message);
        return 1;
    }
    printf("\nShow greedy steps? (y/n): ");
    char answer[16];
    bool show_steps = false;
    if (fgets(answer, sizeof(answer), stdin)) {
        show_steps = (answer[0] == 'y' || answer[0] == 'Y');
    }

    decrypt_message_verbose(&key, ciphertext, decrypted, show_steps);

    printf("\nDecrypted bits: ");
    for (size_t i = 0; i < key.n; i++) printf("%d", decrypted[i]);
    printf("\n");

    mpz_clear(ciphertext);
    keygen_clear(&key);
    free(message);
    free(decrypted);
    return 0;


}
