#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "wolfssl/wolfcrypt/settings.h"
#include <wolfssl/wolfcrypt/rsa.h>


#define AUTH_TAG_LENGTH 16

uint64_t start, end, cycles;
int main(int argc, char **argv)
{
    
    stdio_init_all();
    
    wolfSSL_Init();
    wolfSSL_Debugging_ON();
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }

    RsaKey key;
    WC_RNG rng;
    int ret = 0;
    long e = 65537;

    ret = wc_InitRsaKey(&key, NULL);
    printf("wc_InitRsaKey ret: %d\n", ret);
    ret = wc_InitRng(&rng);
    printf("wc_InitRng ret: %d\n", ret);
    ret = wc_RsaSetRNG(&key, &rng);
    printf("wc_RsaSetRNG ret: %d\n", ret);
    // generate 2048 bit long private key
    ret = wc_MakeRsaKey(&key, 2048, e, &rng);
    printf("wc_MakeRsaKey ret: %d\n", ret);
    if( ret != 0 ) {
        printf("Error");
    }
    else printf("Worked");
    for (int i = 0; i < 5; i++) {
        printf("Worked\n");
        sleep_ms(1000);
    }
    byte pubKeyDer[4096];
    word32 pubKeySz = sizeof(pubKeyDer);

    ret = wc_RsaKeyToPublicDer(&key, pubKeyDer, pubKeySz);
    if (ret > 0) {
        printf("Exported public key, size = %d bytes\n", ret);
    
    }
    byte plain[32] = "Hello RSA on Pico 2W!";
    byte cipher[256];
    byte decrypted[256];
    word32 cipherLen, decryptedLen;

    /* Encrypt */
    cipherLen = wc_RsaPublicEncrypt(plain, sizeof(plain), cipher, sizeof(cipher), &key, &rng);
    printf("RSA encrypt ret = %d\n", cipherLen);

    /* Decrypt */
    decryptedLen = wc_RsaPrivateDecrypt(cipher, cipherLen, decrypted, sizeof(decrypted), &key);
    printf("RSA decrypt ret = %d\n", decryptedLen);

    if (decryptedLen > 0 && memcmp(plain, decrypted, sizeof(plain)) == 0) {
        printf("RSA roundtrip SUCCESS!\n");
    } else {
        printf("RSA roundtrip FAILED!\n");
    }
}