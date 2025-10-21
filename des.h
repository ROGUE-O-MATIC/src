/*
 * DES (Data Encryption Standard) Implementation
 * Header file
 */

#ifndef DES_H
#define DES_H

#include <stdint.h>
#include <stddef.h>

/* DES uses 64-bit blocks and 56-bit keys (+ 8 parity bits = 64 bits total) */
#define DES_BLOCK_SIZE 8
#define DES_KEY_SIZE 8

/* DES context structure */
typedef struct {
    uint64_t subkeys[16];  /* 16 round subkeys */
} DES_Context;

/* Function prototypes */

/**
 * Initialize DES context with a key
 * @param ctx DES context to initialize
 * @param key 64-bit key (8 bytes, only 56 bits used)
 */
void des_init(DES_Context *ctx, const uint8_t key[DES_KEY_SIZE]);

/**
 * Encrypt a single 64-bit block using DES
 * @param ctx DES context with initialized keys
 * @param plaintext Input block (8 bytes)
 * @param ciphertext Output block (8 bytes)
 */
void des_encrypt_block(const DES_Context *ctx, const uint8_t plaintext[DES_BLOCK_SIZE],
                       uint8_t ciphertext[DES_BLOCK_SIZE]);

/**
 * Decrypt a single 64-bit block using DES
 * @param ctx DES context with initialized keys
 * @param ciphertext Input block (8 bytes)
 * @param plaintext Output block (8 bytes)
 */
void des_decrypt_block(const DES_Context *ctx, const uint8_t ciphertext[DES_BLOCK_SIZE],
                       uint8_t plaintext[DES_BLOCK_SIZE]);

/**
 * Encrypt data in ECB mode (Electronic Codebook)
 * Note: ECB is not secure for most uses, use CBC or CTR mode instead
 * @param ctx DES context
 * @param input Input data (must be multiple of 8 bytes)
 * @param output Output buffer
 * @param length Length of data in bytes (must be multiple of 8)
 */
void des_ecb_encrypt(const DES_Context *ctx, const uint8_t *input,
                     uint8_t *output, size_t length);

/**
 * Decrypt data in ECB mode
 * @param ctx DES context
 * @param input Input data (must be multiple of 8 bytes)
 * @param output Output buffer
 * @param length Length of data in bytes (must be multiple of 8)
 */
void des_ecb_decrypt(const DES_Context *ctx, const uint8_t *input,
                     uint8_t *output, size_t length);

/**
 * Encrypt data in CBC mode (Cipher Block Chaining)
 * @param ctx DES context
 * @param iv Initialization vector (8 bytes)
 * @param input Input data (must be multiple of 8 bytes)
 * @param output Output buffer
 * @param length Length of data in bytes (must be multiple of 8)
 */
void des_cbc_encrypt(const DES_Context *ctx, const uint8_t iv[DES_BLOCK_SIZE],
                     const uint8_t *input, uint8_t *output, size_t length);

/**
 * Decrypt data in CBC mode
 * @param ctx DES context
 * @param iv Initialization vector (8 bytes)
 * @param input Input data (must be multiple of 8 bytes)
 * @param output Output buffer
 * @param length Length of data in bytes (must be multiple of 8)
 */
void des_cbc_decrypt(const DES_Context *ctx, const uint8_t iv[DES_BLOCK_SIZE],
                     const uint8_t *input, uint8_t *output, size_t length);

#endif /* DES_H */
