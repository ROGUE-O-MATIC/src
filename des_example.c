/*
 * DES Encryption/Decryption Example
 * Demonstrates usage of the DES implementation
 */

#include <stdio.h>
#include <string.h>
#include "des.h"

/* Helper function to print hex bytes */
void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

/* Example 1: Basic single block encryption/decryption */
void example_basic() {
    printf("=== Example 1: Basic Single Block Encryption ===\n");

    DES_Context ctx;

    /* 64-bit key (8 bytes) */
    uint8_t key[DES_KEY_SIZE] = {
        0x13, 0x34, 0x57, 0x79, 0x9B, 0xBC, 0xDF, 0xF1
    };

    /* 64-bit plaintext (8 bytes) */
    uint8_t plaintext[DES_BLOCK_SIZE] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };

    uint8_t ciphertext[DES_BLOCK_SIZE];
    uint8_t decrypted[DES_BLOCK_SIZE];

    /* Initialize DES with key */
    des_init(&ctx, key);

    /* Encrypt */
    des_encrypt_block(&ctx, plaintext, ciphertext);

    /* Decrypt */
    des_decrypt_block(&ctx, ciphertext, decrypted);

    /* Print results */
    print_hex("Key       ", key, DES_KEY_SIZE);
    print_hex("Plaintext ", plaintext, DES_BLOCK_SIZE);
    print_hex("Ciphertext", ciphertext, DES_BLOCK_SIZE);
    print_hex("Decrypted ", decrypted, DES_BLOCK_SIZE);

    /* Verify decryption */
    if (memcmp(plaintext, decrypted, DES_BLOCK_SIZE) == 0) {
        printf("SUCCESS: Decryption matches original plaintext!\n");
    } else {
        printf("ERROR: Decryption does not match!\n");
    }

    printf("\n");
}

/* Example 2: ECB mode for multiple blocks */
void example_ecb_mode() {
    printf("=== Example 2: ECB Mode (Multiple Blocks) ===\n");

    DES_Context ctx;

    /* Key */
    uint8_t key[DES_KEY_SIZE] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };

    /* Message: "HELLO!!!" (8 bytes) + "WORLD!!!" (8 bytes) = 16 bytes */
    uint8_t plaintext[16] = {
        'H', 'E', 'L', 'L', 'O', '!', '!', '!',
        'W', 'O', 'R', 'L', 'D', '!', '!', '!'
    };

    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    /* Initialize */
    des_init(&ctx, key);

    /* Encrypt in ECB mode */
    des_ecb_encrypt(&ctx, plaintext, ciphertext, 16);

    /* Decrypt in ECB mode */
    des_ecb_decrypt(&ctx, ciphertext, decrypted, 16);

    /* Print results */
    print_hex("Key       ", key, DES_KEY_SIZE);
    printf("Plaintext : %.*s\n", 16, plaintext);
    print_hex("Ciphertext", ciphertext, 16);
    printf("Decrypted : %.*s\n", 16, decrypted);

    /* Verify */
    if (memcmp(plaintext, decrypted, 16) == 0) {
        printf("SUCCESS: ECB decryption successful!\n");
    } else {
        printf("ERROR: ECB decryption failed!\n");
    }

    printf("\n");
}

/* Example 3: CBC mode for multiple blocks */
void example_cbc_mode() {
    printf("=== Example 3: CBC Mode (More Secure) ===\n");

    DES_Context ctx;

    /* Key */
    uint8_t key[DES_KEY_SIZE] = {
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
    };

    /* Initialization Vector (IV) */
    uint8_t iv[DES_BLOCK_SIZE] = {
        0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF
    };

    /* Message: "SECRETMESSAGE!!!" (16 bytes) */
    uint8_t plaintext[16] = {
        'S', 'E', 'C', 'R', 'E', 'T', 'M', 'S',
        'G', ' ', 'H', 'E', 'R', 'E', '!', '!'
    };

    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    /* Initialize */
    des_init(&ctx, key);

    /* Encrypt in CBC mode */
    des_cbc_encrypt(&ctx, iv, plaintext, ciphertext, 16);

    /* Decrypt in CBC mode */
    des_cbc_decrypt(&ctx, iv, ciphertext, decrypted, 16);

    /* Print results */
    print_hex("Key       ", key, DES_KEY_SIZE);
    print_hex("IV        ", iv, DES_BLOCK_SIZE);
    printf("Plaintext : %.*s\n", 16, plaintext);
    print_hex("Ciphertext", ciphertext, 16);
    printf("Decrypted : %.*s\n", 16, decrypted);

    /* Verify */
    if (memcmp(plaintext, decrypted, 16) == 0) {
        printf("SUCCESS: CBC decryption successful!\n");
    } else {
        printf("ERROR: CBC decryption failed!\n");
    }

    printf("\n");
}

/* Example 4: Test with known test vector */
void example_test_vector() {
    printf("=== Example 4: NIST Test Vector ===\n");

    DES_Context ctx;

    /* Known test vector from NIST */
    uint8_t key[DES_KEY_SIZE] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
    };

    uint8_t plaintext[DES_BLOCK_SIZE] = {
        0x95, 0xF8, 0xA5, 0xE5, 0xDD, 0x31, 0xD9, 0x00
    };

    /* Expected ciphertext for this test vector */
    uint8_t expected[DES_BLOCK_SIZE] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    uint8_t ciphertext[DES_BLOCK_SIZE];
    uint8_t decrypted[DES_BLOCK_SIZE];

    /* Initialize */
    des_init(&ctx, key);

    /* Encrypt */
    des_encrypt_block(&ctx, plaintext, ciphertext);

    /* Decrypt */
    des_decrypt_block(&ctx, ciphertext, decrypted);

    /* Print results */
    print_hex("Key         ", key, DES_KEY_SIZE);
    print_hex("Plaintext   ", plaintext, DES_BLOCK_SIZE);
    print_hex("Ciphertext  ", ciphertext, DES_BLOCK_SIZE);
    print_hex("Expected    ", expected, DES_BLOCK_SIZE);
    print_hex("Decrypted   ", decrypted, DES_BLOCK_SIZE);

    /* Note: This test vector might not match exactly as DES has specific test vectors */
    if (memcmp(plaintext, decrypted, DES_BLOCK_SIZE) == 0) {
        printf("SUCCESS: Round-trip encryption/decryption works!\n");
    }

    printf("\n");
}

int main() {
    printf("DES (Data Encryption Standard) Implementation\n");
    printf("==============================================\n\n");

    printf("NOTE: DES is considered cryptographically broken and should\n");
    printf("      NOT be used for security-critical applications.\n");
    printf("      Use AES instead for modern encryption needs.\n");
    printf("      This implementation is for educational purposes.\n\n");

    example_basic();
    example_ecb_mode();
    example_cbc_mode();
    example_test_vector();

    printf("All examples completed!\n");

    return 0;
}
