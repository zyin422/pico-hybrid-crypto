#include <stdio.h>
#include <stdint.h>

#include "wolfssl/wolfcrypt/settings.h"
#include <wolfssl/ssl.h>
#include <wolfcrypt/test/test.h>

#include "pico/stdlib.h"

#include "wolfssl/wolfcrypt/rsa.h"

#define AUTH_TAG_LENGTH 16

uint64_t start, end, cycles;
RsaKey* key = NULL;
word32 idx = 0;
WC_RNG rng;
byte* derBuf = NULL;
int derSz = 0, ret;

int main(int argc, char **argv)
{
    
    stdio_init_all();
    
    wolfSSL_Init();
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }

    key = malloc(sizeof(RsaKey));

    wc_InitRng(&rng);
    wc_InitRsaKey(key, NULL);
    ret = wc_RsaSetRNG(&key, &rng);
    if (ret != 0) {
        printf("Set RSA RNG failed %d\n", ret);
        return 0;
    }
    else printf("Success rng\n");
    printf("min: %d", RSA_MIN_SIZE);    
    ret = wc_MakeRsaKey(key, 2048, 65537, &rng);
    if (ret != 0) {
        printf("Make Key failed %d\n", ret);
        return 0;
    }

    byte input[] = "Hello RSA!";
    byte encrypted[256];
    printf("%d", sizeof(input) - 1);
    ret = wc_RsaPublicEncrypt(input, sizeof(input) - 1, encrypted, sizeof(encrypted), &key, &rng);
    if (ret > 0) {
        printf("Encrypted length = %d\n", ret);
        for (int i = 0; i < ret; i++) {
            printf("%02X", encrypted[i]);
        }
        printf("\n");
    }

    for (int i = 0; i < 5; i++) {
        printf("Worked\n");
        sleep_ms(1000);
    }


}