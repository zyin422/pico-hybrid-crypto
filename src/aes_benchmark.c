/* bench_main.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <stdio.h>
#include <stdint.h>

#include "wolfssl/wolfcrypt/settings.h"
#include <wolfssl/ssl.h>
#include <wolfcrypt/test/test.h>

#include "pico/stdlib.h"

#include "wolfssl/wolfcrypt/aes.h"

#define AUTH_TAG_LENGTH 16

uint64_t start, end, cycles;

int main(int argc, char **argv)
{
    
    stdio_init_all();

    wolfSSL_Init();
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }
    Aes aes;
    int ret;

    /* 256-bit key (32 bytes) */
    const byte key[32] = {
        0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
        0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
        0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
        0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
    };

    /* 96-bit IV (12 bytes) — common for GCM */
    const byte iv[12] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b
    };

    /* Example Authenticated Data (AAD) */
    const byte authIn[] = { 0x01, 0x02, 0x03, 0x04 };

    /* Example plaintext (multiple of AES block size not required for GCM) */

    for (int i = 0; i < 5; i++) {
        const byte plain[] = "12345678901234561234567890123456";
        byte cipher[sizeof(plain)];
        byte decrypted[sizeof(plain)];
        byte authTag[AUTH_TAG_LENGTH];

        /* Initialize AES context */
        if ((ret = wc_AesInit(&aes, NULL, INVALID_DEVID)) != 0) {
            printf("AES init failed, ret = %d\n", ret);
            return -1;
        }

        /* Set AES-GCM key */
        if ((ret = wc_AesGcmSetKey(&aes, key, sizeof(key))) != 0) {
            printf("AES-GCM key set failed, ret = %d\n", ret);
            return -1;
        }


        /* Encrypt */
        start = timer_hw->timerawl | ((uint64_t)timer_hw->timerawh << 32);
        ret = wc_AesGcmEncrypt(&aes,
            cipher, plain, sizeof(plain),
            iv, sizeof(iv),
            authTag, sizeof(authTag),
            authIn, sizeof(authIn));
        if (ret != 0) {
            printf("AES-GCM encryption failed, ret = %d\n", ret);
            return -1;
        }
        end = timer_hw->timerawl | ((uint64_t)timer_hw->timerawh << 32);
        cycles = end - start;
        printf("Encryption took: %llu microseconds\n", cycles);

        printf("Ciphertext: ");
        for (int i = 0; i < sizeof(cipher); i++) printf("%02X", cipher[i]);
        printf("\nAuthTag:    ");
        for (int i = 0; i < sizeof(authTag); i++) printf("%02X", authTag[i]);
        printf("\n");

        /* Decrypt */
        start = timer_hw->timerawl | ((uint64_t)timer_hw->timerawh << 32);
        ret = wc_AesGcmDecrypt(&aes,
            decrypted, cipher, sizeof(cipher),
            iv, sizeof(iv),
            authTag, sizeof(authTag),
            authIn, sizeof(authIn));
        if (ret != 0) {
            printf("AES-GCM decryption/auth failed, ret = %d\n", ret);
            return -1;
        }

        printf("Decrypted:  %s\n", decrypted);
        end = timer_hw->timerawl | ((uint64_t)timer_hw->timerawh << 32);
        cycles = end - start;
        printf("Decryption took: %llu microseconds\n", cycles);

        wc_AesFree(&aes);
        printf("ENDED\n\n");
    }
    for (int i = 0; i < 5; i++) {
        printf("SLEEP\n");
        sleep_ms(1000);
    }
    return 0;
    // int i;
    // int ret;


    
    // printf("End\n");
    // return 0;
}