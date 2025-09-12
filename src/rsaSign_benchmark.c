#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>

#define MSG_BITS 128       // 128, 256, 1024, 4096
#define MSG_BYTES (MSG_BITS / 8)
#define RSA_BITS 2048      // 512, 1024, 2048, 4096
#define RSA_EXP 65537
#define NUM_ITER 5

int main() {
    stdio_init_all();
    for (int i = 0; i < 5; i++) { 
        printf("SLEEP\n"); 
        sleep_ms(1000); 
    }
    int rc = 0;
    RsaKey key;
    WC_RNG rng;
    byte *sig = NULL;
    word32 sig_len = 0;
    uint64_t start, end;
    byte msg[MSG_BYTES];

    // Initialize message
    for (int i = 0; i < MSG_BYTES; i++) msg[i] = i & 0xFF;

    // Initialize RNG
    wc_InitRng(&rng);

    // Initialize RSA key
    wc_InitRsaKey(&key, NULL);

    for (int iter = 1; iter <= NUM_ITER; iter++) {
        printf("=== Iteration %d ===\n", iter);

        // Key generation
        start = time_us_64();
        rc = wc_MakeRsaKey(&key, RSA_BITS, RSA_EXP, &rng);
        end = time_us_64();
        if (rc != 0) { printf("RSA key generation failed: %d\n", rc); goto cleanup; }
        printf("RSA-PSS key generation (%d bits) took: %llu us\n", RSA_BITS, end - start);

        // Hash message first (SHA-256)
        byte hash[SHA256_DIGEST_SIZE];
        wc_Sha256Hash(msg, MSG_BYTES, hash);

        // Allocate signature buffer (RSA_BITS / 8 bytes)
        sig_len = RSA_BITS / 8;
        if (sig) free(sig);
        sig = malloc(sig_len);

        // Signing with RSA-PSS
        start = time_us_64();
        rc = wc_RsaPSS_Sign(hash, sizeof(hash), sig, sig_len, WC_HASH_TYPE_SHA256, 
                             WC_MGF1SHA256, &key, &rng);
        end = time_us_64();
        if (rc < 0) { printf("RSA-PSS signing failed: %d\n", rc); goto cleanup; }
        printf("RSA-PSS signing took: %llu us\n", end - start);

        // Verification
        int verified = 0;
        byte recovered[RSA_BITS / 8];
        start = time_us_64();
        rc = wc_RsaPSS_Verify(sig, sig_len, recovered, sizeof(recovered), WC_HASH_TYPE_SHA256, 
                              WC_MGF1SHA256, &key);
        end = time_us_64();
        if (rc < 0) { printf("RSA-PSS verification failed: %d\n", rc); goto cleanup; }
        printf("RSA-PSS verification took: %llu us\n", end - start);
        verified = (rc > 0);
        if (verified) printf("RSA-PSS test PASSED - signature verified! Length is %d\n", rc);
        else printf("RSA-PSS test FAILED - verification error\n");

        printf("\n");
    }

cleanup:
    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    if (sig) free(sig);

    return (rc == 0) ? 0 : 1;
}
