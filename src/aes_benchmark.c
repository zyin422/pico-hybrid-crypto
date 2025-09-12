#include <stdio.h>
#include <stdint.h>
#include "wolfssl/wolfcrypt/aes.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"

/* ---------- Configurable ---------- */
#define AES_BITS 128           // 128 or 256
#define PLAINTEXT_BITS 128     // 128, 256, 1024, 4096
#define AUTH_TAG_LENGTH 16

/* Derived constants */
#define AES_BYTES (AES_BITS / 8)
#define PLAINTEXT_BYTES (PLAINTEXT_BITS / 8)

uint64_t get_time_us() { return time_us_64(); }

int main(void) {
    stdio_init_all();
    for (int i = 0; i < 5; i++) { printf("SLEEP\n"); sleep_ms(1000); }

    Aes aes;
    int ret;
    WC_RNG rng;
    wc_InitRng(&rng);

    /* AES key dynamically generated */
    byte key[AES_BYTES];
    wc_RNG_GenerateBlock(&rng, key, AES_BYTES);

    /* 96-bit IV for GCM */
    byte iv[12];
    wc_RNG_GenerateBlock(&rng, iv, sizeof(iv));

    /* Example Authenticated Data */
    byte authIn[] = {0x01,0x02,0x03,0x04};

    /* Plaintext of configurable size */
    byte plain[PLAINTEXT_BYTES];
    for (int i = 0; i < PLAINTEXT_BYTES; i++) plain[i] = i & 0xFF;

    byte cipher[PLAINTEXT_BYTES];
    byte decrypted[PLAINTEXT_BYTES];
    byte authTag[AUTH_TAG_LENGTH];

    for (int iter = 0; iter < 5; iter++) {
        /* Initialize AES context */
        if ((ret = wc_AesInit(&aes, NULL, INVALID_DEVID)) != 0) { printf("AES init failed\n"); return -1; }

        if ((ret = wc_AesGcmSetKey(&aes, key, AES_BYTES)) != 0) { printf("AES key set failed\n"); return -1; }

        /* Encrypt */
        uint64_t start = get_time_us();
        ret = wc_AesGcmEncrypt(&aes,
            cipher, plain, PLAINTEXT_BYTES,
            iv, sizeof(iv),
            authTag, sizeof(authTag),
            authIn, sizeof(authIn));
        uint64_t end = get_time_us();
        printf("Encryption took: %llu us\n", end - start);

        /* Decrypt */
        start = get_time_us();
        ret = wc_AesGcmDecrypt(&aes,
            decrypted, cipher, PLAINTEXT_BYTES,
            iv, sizeof(iv),
            authTag, sizeof(authTag),
            authIn, sizeof(authIn));
        end = get_time_us();
        printf("Decryption took: %llu us\n", end - start);

        /* Verify */
        if (memcmp(plain, decrypted, PLAINTEXT_BYTES) == 0) printf("Decrypted matches plaintext!\n");
        else printf("Decryption FAILED!\n");

        wc_AesFree(&aes);
        printf("ITERATION END\n\n");
    }

    return 0;
}
