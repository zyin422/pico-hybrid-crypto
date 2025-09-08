#include <stdio.h>
#include <stdint.h>

#include "wolfssl/wolfcrypt/settings.h"
#include <wolfssl/ssl.h>
#include <wolfcrypt/test/test.h>

#include "pico/stdlib.h"

#include "wolfssl/wolfcrypt/rsa.h"
#include <string.h>

// Check stack usage
void check_stack_usage() {
    extern char __StackBottom, __StackTop;
    char stack_var;
    uintptr_t stack_ptr = (uintptr_t)&stack_var;
    uintptr_t stack_bottom = (uintptr_t)&__StackBottom;
    uintptr_t stack_top = (uintptr_t)&__StackTop;
    
    printf("Stack bottom: 0x%08x\n", (uint32_t)stack_bottom);
    printf("Stack top: 0x%08x\n", (uint32_t)stack_top);
    printf("Current SP: 0x%08x\n", (uint32_t)stack_ptr);
    printf("Stack size: %d bytes\n", (int)(stack_top - stack_bottom));
    printf("Stack used: %d bytes\n", (int)(stack_top - stack_ptr));
    printf("Stack remaining: %d bytes\n", (int)(stack_ptr - stack_bottom));
}

// Check heap usage - working version for Pico
void check_heap_usage() {
    extern char __StackLimit;  // This is the heap limit
    extern char __end__;       // End of BSS, start of heap
    
    printf("Heap start (end of BSS): 0x%08x\n", (uint32_t)(uintptr_t)&__end__);
    printf("Heap limit (stack bottom): 0x%08x\n", (uint32_t)(uintptr_t)&__StackLimit);
    printf("Max available heap: %d bytes\n", (int)((uintptr_t)&__StackLimit - (uintptr_t)&__end__));
    
    // Test current heap usage by trying malloc
    void *test_ptr = malloc(16);
    if (test_ptr) {
        printf("Heap allocation test successful at: 0x%08x\n", (uint32_t)(uintptr_t)test_ptr);
        free(test_ptr);
    } else {
        printf("Heap allocation test failed!\n");
    }
}

// Simple memory stress test without __brkval
void memory_stress_test() {
    printf("\n=== Memory Stress Test ===\n");
    
    // Try allocating increasingly large blocks
    size_t sizes[] = {1024, 2048, 4096, 8192, 16384, 32768, 65536};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (size_t i = 0; i < num_sizes; i++) {
        void *ptr = malloc(sizes[i]);
        if (ptr) {
            printf("✓ Successfully allocated %zu bytes at 0x%08x\n", 
                   sizes[i], (uint32_t)(uintptr_t)ptr);
            // Write to it to make sure it's real
            memset(ptr, 0xAA, sizes[i]);
            free(ptr);
        } else {
            printf("✗ Failed to allocate %zu bytes\n", sizes[i]);
            break;
        }
    }
}

// Test function to simulate RSA-like memory usage
void simulate_rsa_memory_usage() {
    wolfSSL_Init();
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }

    RsaKey priv;
    WC_RNG rng;
    int ret = 0;
    long e = 65537; // standard value to use for exponent

    ret = wc_InitRsaKey(&priv, NULL);
    printf("wc_InitRsaKey ret: %d\n", ret);
    ret = wc_InitRng(&rng);
    printf("wc_InitRng ret: %d\n", ret);
    ret = wc_RsaSetRNG(&priv, &rng);
    printf("wc_RsaSetRNG ret: %d\n", ret);
    // generate 2048 bit long private key
    ret = wc_MakeRsaKey(&priv, 2048, e, &rng);
    printf("wc_MakeRsaKey ret: %d\n", ret);
    if( ret != 0 ) {
        printf("Error");
        // error generating private key
    }
    else printf("Worked");
}

// Stack depth test
void recursive_stack_test(int depth, int max_depth) {
    char local_buffer[256]; // 256 bytes per recursion level
    memset(local_buffer, depth & 0xFF, sizeof(local_buffer));
    
    printf("Stack depth %d/%d, buffer at 0x%08x\n", 
           depth, max_depth, (uint32_t)(uintptr_t)local_buffer);
    
    if (depth < max_depth) {
        recursive_stack_test(depth + 1, max_depth);
    }
    
    // Use the buffer to prevent optimization
    volatile char check = local_buffer[0];
    (void)check;
}

int main() {
    stdio_init_all();
    
    // Wait for USB serial connection
    printf("Waiting for serial connection...\n");
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }
    
    printf("=== Pico Memory Analysis ===\n");
    printf("Analyzing memory layout and capacity...\n\n");
    
    // Initial memory state
    printf("=== Initial Memory State ===\n");
    check_stack_usage();
    printf("\n");
    check_heap_usage();
    
    // Test stack usage with local array
    // Test with some stack usage - prevent optimization
    printf("\n=== Stack Usage Test ===\n");
    volatile char big_array[2048];  // volatile prevents optimization
    for(int i = 0; i < 2048; i++) {
        big_array[i] = i & 0xFF;
    }
    printf("After allocating 2KB on stack:\n");
    check_stack_usage();
    printf("Array checksum: %u\n", (unsigned)big_array[1000]); // Use the array
        
    // Test heap allocations
    memory_stress_test();
    
    // Simulate RSA memory requirements
    simulate_rsa_memory_usage();
    
    // Test stack depth (be careful with this)
    printf("\n=== Stack Depth Test ===\n");
    printf("Testing recursive stack usage (5 levels):\n");
    recursive_stack_test(1, 5);
    
    // Final memory state
    printf("\n=== Final Memory State ===\n");
    check_stack_usage();
    printf("\n");
    check_heap_usage();
    
    printf("\n=== Ready for RSA Testing ===\n");
    printf("You can now add your RSA operations here.\n");
    printf("Monitor the output above to see if memory constraints are the issue.\n");
    
    // Insert your RSA makekey/sign operations here
    // Example:
    // printf("Starting RSA key generation...\n");
    // int result = your_rsa_makekey_function();
    // printf("RSA result: %d\n", result);
    
    while (1) {
        sleep_ms(1000);
    }
    
    return 0;
}