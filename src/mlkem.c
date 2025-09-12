#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include <wolfssl/wolfcrypt/mlkem.h>
#include <wolfssl/wolfcrypt/wc_mlkem.h>
#include <wolfssl/wolfcrypt/random.h>

int main() {
    stdio_init_all();
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }
    int ret = 0;
    WC_RNG rng;
    MlKemKey AliceKey;
    MlKemKey Bobkey;
    
    word32 pub_size, ct_size, ss_size;
    unsigned char *alice_pub = NULL;
    unsigned char *ciphertext = NULL;
    unsigned char *shared_secret_alice = NULL;
    unsigned char *shared_secret_bob = NULL;
    
    printf("ML-KEM-512 Key Encapsulation Test\n\n");
    
    // Initialize RNG
    ret = wc_InitRng(&rng);
    if (ret != 0) {
        printf("RNG init failed: %d\n", ret);
        return 1;
    }
    
    // 1. Alice creates ML-KEM-512 key pair
    printf("1. Alice creates ML-KEM-512 key pair\n");
    ret = wc_MlKemKey_Init(&AliceKey, WC_ML_KEM_512, NULL, INVALID_DEVID);
    if (ret != 0) {
        printf("Alice key init failed: %d\n", ret);
        goto cleanup;
    }
    printf("2. Key generation\n");
    
    ret = wc_MlKemKey_MakeKey(&AliceKey, &rng);
    if (ret != 0) {
        printf("Alice key generation failed: %d\n", ret);
        goto cleanup;
    }
    printf("3. Get buffer sizes\n");
    ret = wc_MlKemKey_PublicKeySize(&AliceKey, &pub_size);
    if (ret != 0) {
        printf("Failed to get public key size: %d\n", ret);
        goto cleanup;
    }
    
    printf("4. Get ciphertext sizes\n");
    ret = wc_MlKemKey_CipherTextSize(&AliceKey, &ct_size);
    if (ret != 0) {
        printf("Failed to get ciphertext size: %d\n", ret);
        goto cleanup;
    }
    
    printf("5. Get shared secret sizes\n");
    ret = wc_MlKemKey_SharedSecretSize(&AliceKey, &ss_size);
    if (ret != 0) {
        printf("Failed to get shared secret size: %d\n", ret);
        goto cleanup;
    }
    
    printf("   Public key size: %d bytes\n", pub_size);
    printf("   Ciphertext size: %d bytes\n", ct_size);
    printf("   Shared secret size: %d bytes\n", ss_size);
    
    // Allocate buffers
    alice_pub = malloc(pub_size);
    ciphertext = malloc(ct_size);
    shared_secret_alice = malloc(ss_size);
    shared_secret_bob = malloc(ss_size);
    
    if (!alice_pub || !ciphertext || !shared_secret_alice || !shared_secret_bob) {
        printf("Memory allocation failed\n");
        ret = -1;
        goto cleanup;
    }
    
    printf("6. Alice encodes her public key for Bob\n");
    ret = wc_MlKemKey_EncodePublicKey(&AliceKey, alice_pub, pub_size);
    if (ret != 0) {
        printf("Public key encoding failed: %d\n", ret);
        goto cleanup;
    }
    
    // 3. Bob receives and decodes Alice's public key
    printf("7. Bob decodes Alice's public key\n");
    ret = wc_MlKemKey_Init(&Bobkey, WC_ML_KEM_512, NULL, INVALID_DEVID);
    if (ret != 0) {
        printf("Bob key init failed: %d\n", ret);
        goto cleanup;
    }

    printf("8. Decoding public key\n");
    ret = wc_MlKemKey_DecodePublicKey(&Bobkey, alice_pub, pub_size);
    if (ret != 0) {
        printf("Public key decoding failed: %d\n", ret);
        goto cleanup;
    }
    
    // 4. Bob encapsulates a shared secret
    printf("4. Bob encapsulates shared secret\n");
    ret = wc_MlKemKey_Encapsulate(&Bobkey, ciphertext, shared_secret_bob, &rng);
    if (ret != 0) {
        printf("Encapsulation failed: %d\n", ret);
        goto cleanup;
    }
    
    // 5. Alice decapsulates the shared secret
    printf("5. Alice decapsulates shared secret\n");
    ret = wc_MlKemKey_Decapsulate(&AliceKey, shared_secret_alice, ciphertext, ct_size);
    if (ret != 0) {
        printf("Decapsulation failed: %d\n", ret);
        goto cleanup;
    }
    
    // 6. Compare shared secrets
    printf("6. Comparing shared secrets\n");
    if (memcmp(shared_secret_alice, shared_secret_bob, ss_size) == 0) {
        printf("SUCCESS: Shared secrets match!\n");
        printf("ML-KEM-512 key encapsulation test PASSED\n");
    } else {
        printf("FAILURE: Shared secrets don't match!\n");
        ret = -1;
    }
    
cleanup:
    // Free keys
    wc_MlKemKey_Free(&AliceKey);
    wc_MlKemKey_Free(&Bobkey);
    wc_FreeRng(&rng);
    
    // Free buffers
    if (alice_pub) free(alice_pub);
    if (ciphertext) free(ciphertext);
    if (shared_secret_alice) free(shared_secret_alice);
    if (shared_secret_bob) free(shared_secret_bob);
    
    return (ret == 0) ? 0 : 1;
}