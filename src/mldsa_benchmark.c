#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include <wolfssl/wolfcrypt/sha3.h>
#include <wolfssl/wolfcrypt/dilithium.h>
#include <wolfssl/wolfcrypt/random.h>

#define PLAINTEXT_BITS 4096     // 128, 256, 1024, 4096
#define PLAINTEXT_BYTES (PLAINTEXT_BITS / 8)
#define REPEAT_COUNT 5

int main() {
    stdio_init_all();
    int rc = 0;
    MlDsaKey key;
    WC_RNG rng;
    byte *sig = NULL;
    word32 sig_len = 0;
    int verify_res = 0;
    uint64_t start, end;

    byte msg[PLAINTEXT_BYTES];
    for (int i = 0; i < PLAINTEXT_BYTES; i++) msg[i] = i & 0xFF;

    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }
    
    // Initialize RNG and key
    wc_InitRng(&rng);
    wc_MlDsaKey_Init(&key, NULL, INVALID_DEVID);
    wc_MlDsaKey_SetParams(&key, WC_ML_DSA_44);

    // Repeat the test multiple times
    for (int iter = 0; iter < REPEAT_COUNT; iter++) {
        printf("=== Iteration %d ===\n", iter + 1);

        // Generate key pair
        start = time_us_64();
        rc = wc_MlDsaKey_MakeKey(&key, &rng);
        end = time_us_64();
        if (rc != 0) {
            printf("Key generation failed: %d\n", rc);
            goto cleanup;
        }
        printf("Key generation took: %llu us\n", end - start);
        
        // Allocate signature buffer
        wc_MlDsaKey_GetSigLen(&key, (int*)&sig_len);
        sig = malloc(sig_len);

        // Sign
        start = time_us_64();
        rc = wc_MlDsaKey_Sign(&key, sig, &sig_len, msg, sizeof(msg), &rng);
        end = time_us_64();
        if (rc != 0) {
            printf("Signing failed: %d\n", rc);
            goto cleanup;
        }
        printf("Signing took: %llu us\n", end - start);

        // Verify
        start = time_us_64();
        rc = wc_MlDsaKey_Verify(&key, sig, sig_len, msg, sizeof(msg), &verify_res);
        end = time_us_64();
        printf("Verification took: %llu us\n", end - start);

        if (rc == 0 && verify_res == 1)
            printf("ML-DSA-44 test PASSED - signature verified!\n");
        else
            printf("ML-DSA-44 test FAILED - verification error\n");

        free(sig);
        sig = NULL;
    }

cleanup:
    wc_MlDsaKey_Free(&key);
    wc_FreeRng(&rng);
    if (sig) free(sig);
    
    return (rc == 0 && verify_res == 1) ? 0 : 1;
}
