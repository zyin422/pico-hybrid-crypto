#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#include "wolfssl/wolfcrypt/settings.h"

#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/random.h>


int main(void) {
    stdio_init_all();
    wolfSSL_Init();
    WC_RNG rng;
    wc_InitRng(&rng);
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }

    RsaKey rsaKey;
    wc_InitRsaKey(&rsaKey, NULL);

    uint8_t aesKey128[16];
    uint8_t encBuf[256], decBuf[256];
    word32 encLen, decLen;

    // RSA keygen benchmark
    uint64_t start = time_us_64();
    wc_MakeRsaKey(&rsaKey, 2048, 65537, &rng);
    uint64_t end = time_us_64();
    printf("RSA-2048 keygen: %llu us\n", end - start);

    // Encrypt AES-128 key with RSA
    wc_RNG_GenerateBlock(&rng, aesKey128, 16);
    start = time_us_64();
    encLen = wc_RsaPublicEncrypt(aesKey128, 16, encBuf, sizeof(encBuf), &rsaKey, &rng);
    end = time_us_64();
    printf("RSA-2048 encrypt AES-128: %llu us\n", end - start);

    // Decrypt AES-128 key with RSA
    start = time_us_64();
    decLen = wc_RsaPrivateDecrypt(encBuf, encLen, decBuf, sizeof(decBuf), &rsaKey);
    end = time_us_64();
    printf("RSA-2048 decrypt AES-128: %llu us\n", end - start);
}
