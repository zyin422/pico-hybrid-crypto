#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#include <wolfssl/wolfcrypt/mlkem.h>
#include <wolfssl/wolfcrypt/wc_mlkem.h>
#include <wolfssl/wolfcrypt/random.h>

int main() {
    stdio_init_all();
    sleep_ms(5000); // startup delay


    printf("=== ML-KEM-512 Benchmark ===\n");
    for (int i = 0; i < 5; i++) {
        WC_RNG rng;
        MlKemKey AliceKey, BobKey;
        int ret;
        word32 pub_size, ct_size, ss_size;

        ret = wc_InitRng(&rng);
        if (ret != 0) {
            printf("RNG init failed\n");
            return -1;
        }

        // Init keys
        wc_MlKemKey_Init(&AliceKey, WC_ML_KEM_512, NULL, INVALID_DEVID);
        wc_MlKemKey_Init(&BobKey, WC_ML_KEM_512, NULL, INVALID_DEVID);

        // KeyGen benchmark
        uint64_t start = time_us_64();
        ret = wc_MlKemKey_MakeKey(&AliceKey, &rng);
        uint64_t end = time_us_64();
        printf("KeyGen cycles: %llu\n", (unsigned long long)(end - start));

        // Sizes
        wc_MlKemKey_PublicKeySize(&AliceKey, &pub_size);
        wc_MlKemKey_CipherTextSize(&AliceKey, &ct_size);
        wc_MlKemKey_SharedSecretSize(&AliceKey, &ss_size);

        unsigned char *alice_pub = malloc(pub_size);
        unsigned char *ciphertext = malloc(ct_size);
        unsigned char *ss_alice   = malloc(ss_size);
        unsigned char *ss_bob     = malloc(ss_size);

        // Encode Alice's public key
        wc_MlKemKey_EncodePublicKey(&AliceKey, alice_pub, pub_size);
        wc_MlKemKey_DecodePublicKey(&BobKey, alice_pub, pub_size);

        // Encapsulate benchmark
        start = time_us_64();
        ret = wc_MlKemKey_Encapsulate(&BobKey, ciphertext, ss_bob, &rng);
        end = time_us_64();
        printf("Encaps cycles: %llu\n", (unsigned long long)(end - start));

        // Decapsulate benchmark
        start = time_us_64();
        ret = wc_MlKemKey_Decapsulate(&AliceKey, ss_alice, ciphertext, ct_size);
        end = time_us_64();
        printf("Decaps cycles: %llu\n", (unsigned long long)(end - start));

        // Check correctness
        if (memcmp(ss_alice, ss_bob, ss_size) == 0) {
            printf("Shared secrets match!\n");
        } else {
            printf("Shared secrets mismatch!\n");
        }

        // Cleanup
        wc_MlKemKey_Free(&AliceKey);
        wc_MlKemKey_Free(&BobKey);
        wc_FreeRng(&rng);

        free(alice_pub);
        free(ciphertext);
        free(ss_alice);
        free(ss_bob);
    }

    return 0;
}
