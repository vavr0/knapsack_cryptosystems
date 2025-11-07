#include "keygen.h"
#include "encrypt.h"
#include "decrypt.h"
#include "utils.h"

int main() {
    printf("===knapsack demo===\n");

    KnapsackKey key;
    keygen_init(&key, 8);
    keygen_generate(&key);

    printf("Private key (superincreasing):\n");
    for (size_t i = 0; i < key.n; i++)
        gmp_printf("w[%zu] = %Zd\n", i, key.w[i]);

    printf("\nPublic key:\n");
    for (size_t i = 0; i < key.n; i++)
        gmp_printf("b[%zu] = %Zd\n", i, key.b[i]);

    gmp_printf("\nm = %Zd\nn = %Zd\n", key.m, key.n_mult);

    int message[8] = {1, 0, 1, 1, 0, 1, 0, 0};
    printf("\nPlaintext bits: ");
    for (size_t i = 0; i < 8; i++)
        printf("%d", message[i]);
    printf("\n");

    // Step 3: Encrypt
    mpz_t ciphertext;
    mpz_init(ciphertext);
    encrypt_message(&key, message, ciphertext);

    printf("\nCiphertext:\n");
    gmp_printf("C = %Zd\n", ciphertext);


    int decrypted[8];
    decrypt_message(&key, ciphertext, decrypted);

    printf("\nDecrypted bits: ");
    for (size_t i = 0; i < key.n; i++) printf("%d", decrypted[i]);
    printf("\n");

    mpz_clear(ciphertext);
    keygen_clear(&key);
    return 0;
}
