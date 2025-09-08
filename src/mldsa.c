#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/dilithium.h>
#include <wolfssl/wolfcrypt/random.h>

int main() {
    stdio_init_all();
    wolfSSL_Init();
    int rc = 0;
    MlDsaKey key;
    WC_RNG rng;
    byte *sig = NULL;
    word32 sig_len = 0;
    const char *msg = "test message";
    int verify_res = 0;
    
    // Initialize
    wc_InitRng(&rng);
    wc_MlDsaKey_Init(&key, NULL, INVALID_DEVID);
    
    // Set ML-DSA-44 (Category 2)
    wc_MlDsaKey_SetParams(&key, 2);
    
    // Generate key pair
    rc = wc_MlDsaKey_MakeKey(&key, &rng);
    if (rc != 0) {
        printf("Key generation failed: %d\n", rc);
        goto cleanup;
    }
    
    // Get signature length and allocate
    wc_MlDsaKey_GetSigLen(&key, (int*)&sig_len);
    sig = malloc(sig_len);
    
    // Sign message
    rc = wc_MlDsaKey_Sign(&key, sig, &sig_len, (const byte*)msg, strlen(msg), &rng);
    if (rc != 0) {
        printf("Signing failed: %d\n", rc);
        goto cleanup;
    }
    
    // Verify signature
    rc = wc_MlDsaKey_Verify(&key, sig, sig_len, (const byte*)msg, strlen(msg), &verify_res);
    
    if (rc == 0 && verify_res == 1) {
        printf("ML-DSA-44 test PASSED - signature verified!\n");
    } else {
        printf("ML-DSA-44 test FAILED - verification error\n");
    }
    
cleanup:
    wc_MlDsaKey_Free(&key);
    wc_FreeRng(&rng);
    if (sig) free(sig);
    
    return (rc == 0 && verify_res == 1) ? 0 : 1;
}