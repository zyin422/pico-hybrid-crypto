#include <stdio.h>
#include <stdint.h>

#include "wolfssl/wolfcrypt/settings.h"
#include <wolfssl/ssl.h>
#include <wolfcrypt/test/test.h>

#include "pico/stdlib.h"

#include "wolfssl/wolfcrypt/rsa.h"
#include <wolfssl/wolfcrypt/ecc.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/ecc.h>

int main(void) {
    stdio_init_all();
    wolfSSL_Init();
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }

    int ret;
    WC_RNG rng;
    ecc_key eccPriv, eccPeer;

    // Initialize RNG
    ret = wc_InitRng(&rng);
    printf("wc_InitRng ret: %d\n", ret);

    // Initialize ECC keys
    ret = wc_ecc_init(&eccPriv);
    printf("wc_ecc_init(priv) ret: %d\n", ret);
    ret = wc_ecc_init(&eccPeer);
    printf("wc_ecc_init(peer) ret: %d\n", ret);

    // Generate ECC private key (384-bit SECP384R1)
    ret = wc_ecc_make_key(&rng, 48, &eccPriv); // 48 bytes = 384 bits
    printf("wc_ecc_make_key(priv) ret: %d\n", ret);

    // Export public key from private key
    byte pubBuf[100];
    word32 pubLen = sizeof(pubBuf);
    ret = wc_ecc_export_x963(&eccPriv, pubBuf, &pubLen);
    printf("wc_ecc_export_x963 ret: %d\n", ret);

    // Import public key to peer key structure
    ret = wc_ecc_import_x963(pubBuf, pubLen, &eccPeer);
    printf("wc_ecc_import_x963 ret: %d\n", ret);

    // Derive shared secret using private key and peer's public key
    byte sharedSecret[48];   // 48 bytes = 384-bit curve
    word32 secretLen = sizeof(sharedSecret);
    ret = wc_ecc_shared_secret(&eccPriv, &eccPeer, sharedSecret, &secretLen);
    printf("wc_ecc_shared_secret ret: %d\n", ret);

    printf("Shared secret: ");
    for (int i = 0; i < secretLen; i++) {
        printf("%02X", sharedSecret[i]);
    }
    printf("\n");

    // Clean up
    wc_ecc_free(&eccPriv);
    wc_ecc_free(&eccPeer);
    wc_FreeRng(&rng);

    return 0;
}