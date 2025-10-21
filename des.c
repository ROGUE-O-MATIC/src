/*
 * DES (Data Encryption Standard) Implementation
 * Based on FIPS PUB 46-3
 */

#include "des.h"
#include <string.h>

/* Initial Permutation (IP) */
static const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

/* Final Permutation (IP^-1) */
static const int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};

/* Expansion Permutation (E) - expands 32 bits to 48 bits */
static const int E[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

/* Permutation P */
static const int P[32] = {
    16,  7, 20, 21,
    29, 12, 28, 17,
     1, 15, 23, 26,
     5, 18, 31, 10,
     2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
};

/* Permuted Choice 1 (PC1) - selects 56 bits from 64-bit key */
static const int PC1[56] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};

/* Permuted Choice 2 (PC2) - selects 48 bits from 56-bit key */
static const int PC2[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

/* Number of left shifts for each round */
static const int SHIFTS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 1
};

/* S-boxes (Substitution boxes) */
static const int S[8][4][16] = {
    /* S1 */
    {
        {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
        { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
        { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
        {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}
    },
    /* S2 */
    {
        {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
        { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
        { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
        {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}
    },
    /* S3 */
    {
        {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
        {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
        {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
        { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
    },
    /* S4 */
    {
        { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
        {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
        {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
        { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
    },
    /* S5 */
    {
        { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
        {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
        { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
        {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}
    },
    /* S6 */
    {
        {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
        {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
        { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
        { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}
    },
    /* S7 */
    {
        { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
        {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
        { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
        { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}
    },
    /* S8 */
    {
        {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
        { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
        { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
        { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}
    }
};

/* Helper function: Get bit at position from byte array */
static inline int get_bit(const uint8_t *data, int pos) {
    int byte_pos = pos / 8;
    int bit_pos = 7 - (pos % 8);
    return (data[byte_pos] >> bit_pos) & 1;
}

/* Helper function: Set bit at position in byte array */
static inline void set_bit(uint8_t *data, int pos, int val) {
    int byte_pos = pos / 8;
    int bit_pos = 7 - (pos % 8);
    if (val)
        data[byte_pos] |= (1 << bit_pos);
    else
        data[byte_pos] &= ~(1 << bit_pos);
}

/* Helper function: Convert byte array to 64-bit integer */
static uint64_t bytes_to_uint64(const uint8_t *bytes) {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 8) | bytes[i];
    }
    return result;
}

/* Helper function: Convert 64-bit integer to byte array */
static void uint64_to_bytes(uint64_t val, uint8_t *bytes) {
    for (int i = 7; i >= 0; i--) {
        bytes[i] = val & 0xFF;
        val >>= 8;
    }
}

/* Permute bits according to a permutation table */
static uint64_t permute(uint64_t input, const int *table, int n) {
    uint64_t output = 0;
    for (int i = 0; i < n; i++) {
        if (input & (1ULL << (64 - table[i]))) {
            output |= (1ULL << (n - 1 - i));
        }
    }
    return output;
}

/* Left circular shift */
static uint32_t left_shift(uint32_t value, int shifts, int bits) {
    uint32_t mask = (1 << bits) - 1;
    return ((value << shifts) | (value >> (bits - shifts))) & mask;
}

/* F function: core of DES algorithm */
static uint32_t f_function(uint32_t R, uint64_t subkey) {
    /* Expand R from 32 to 48 bits */
    uint64_t expanded = 0;
    for (int i = 0; i < 48; i++) {
        if (R & (1UL << (32 - E[i]))) {
            expanded |= (1ULL << (47 - i));
        }
    }

    /* XOR with subkey */
    uint64_t xored = expanded ^ subkey;

    /* Apply S-boxes */
    uint32_t output = 0;
    for (int i = 0; i < 8; i++) {
        /* Extract 6 bits for this S-box */
        int bits = (xored >> (42 - i * 6)) & 0x3F;

        /* Calculate row and column */
        int row = ((bits & 0x20) >> 4) | (bits & 0x01);
        int col = (bits >> 1) & 0x0F;

        /* Get S-box value */
        int s_val = S[i][row][col];

        /* Add to output */
        output = (output << 4) | s_val;
    }

    /* Apply permutation P */
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        if (output & (1UL << (32 - P[i]))) {
            result |= (1UL << (31 - i));
        }
    }

    return result;
}

/* Initialize DES context with key */
void des_init(DES_Context *ctx, const uint8_t key[DES_KEY_SIZE]) {
    uint64_t key64 = bytes_to_uint64(key);

    /* Apply PC1 permutation to get 56-bit key */
    uint64_t permuted_key = permute(key64, PC1, 56);

    /* Split into two 28-bit halves */
    uint32_t C = (permuted_key >> 28) & 0x0FFFFFFF;
    uint32_t D = permuted_key & 0x0FFFFFFF;

    /* Generate 16 subkeys */
    for (int i = 0; i < 16; i++) {
        /* Perform left circular shifts */
        C = left_shift(C, SHIFTS[i], 28);
        D = left_shift(D, SHIFTS[i], 28);

        /* Combine C and D */
        uint64_t CD = ((uint64_t)C << 28) | D;

        /* Apply PC2 permutation to get 48-bit subkey */
        uint64_t subkey = 0;
        for (int j = 0; j < 48; j++) {
            if (CD & (1ULL << (56 - PC2[j]))) {
                subkey |= (1ULL << (47 - j));
            }
        }

        ctx->subkeys[i] = subkey;
    }
}

/* Encrypt a single block */
void des_encrypt_block(const DES_Context *ctx, const uint8_t plaintext[DES_BLOCK_SIZE],
                       uint8_t ciphertext[DES_BLOCK_SIZE]) {
    uint64_t block = bytes_to_uint64(plaintext);

    /* Initial permutation */
    block = permute(block, IP, 64);

    /* Split into left and right halves */
    uint32_t L = (block >> 32) & 0xFFFFFFFF;
    uint32_t R = block & 0xFFFFFFFF;

    /* 16 rounds */
    for (int i = 0; i < 16; i++) {
        uint32_t temp = R;
        R = L ^ f_function(R, ctx->subkeys[i]);
        L = temp;
    }

    /* Combine R and L (note: R and L are swapped) */
    block = ((uint64_t)R << 32) | L;

    /* Final permutation */
    block = permute(block, FP, 64);

    uint64_to_bytes(block, ciphertext);
}

/* Decrypt a single block */
void des_decrypt_block(const DES_Context *ctx, const uint8_t ciphertext[DES_BLOCK_SIZE],
                       uint8_t plaintext[DES_BLOCK_SIZE]) {
    uint64_t block = bytes_to_uint64(ciphertext);

    /* Initial permutation */
    block = permute(block, IP, 64);

    /* Split into left and right halves */
    uint32_t L = (block >> 32) & 0xFFFFFFFF;
    uint32_t R = block & 0xFFFFFFFF;

    /* 16 rounds (with reversed subkeys) */
    for (int i = 15; i >= 0; i--) {
        uint32_t temp = R;
        R = L ^ f_function(R, ctx->subkeys[i]);
        L = temp;
    }

    /* Combine R and L (note: R and L are swapped) */
    block = ((uint64_t)R << 32) | L;

    /* Final permutation */
    block = permute(block, FP, 64);

    uint64_to_bytes(block, plaintext);
}

/* ECB mode encryption */
void des_ecb_encrypt(const DES_Context *ctx, const uint8_t *input,
                     uint8_t *output, size_t length) {
    for (size_t i = 0; i < length; i += DES_BLOCK_SIZE) {
        des_encrypt_block(ctx, input + i, output + i);
    }
}

/* ECB mode decryption */
void des_ecb_decrypt(const DES_Context *ctx, const uint8_t *input,
                     uint8_t *output, size_t length) {
    for (size_t i = 0; i < length; i += DES_BLOCK_SIZE) {
        des_decrypt_block(ctx, input + i, output + i);
    }
}

/* CBC mode encryption */
void des_cbc_encrypt(const DES_Context *ctx, const uint8_t iv[DES_BLOCK_SIZE],
                     const uint8_t *input, uint8_t *output, size_t length) {
    uint8_t prev_block[DES_BLOCK_SIZE];
    uint8_t xor_block[DES_BLOCK_SIZE];

    memcpy(prev_block, iv, DES_BLOCK_SIZE);

    for (size_t i = 0; i < length; i += DES_BLOCK_SIZE) {
        /* XOR with previous ciphertext block (or IV for first block) */
        for (int j = 0; j < DES_BLOCK_SIZE; j++) {
            xor_block[j] = input[i + j] ^ prev_block[j];
        }

        /* Encrypt */
        des_encrypt_block(ctx, xor_block, output + i);

        /* Save ciphertext for next round */
        memcpy(prev_block, output + i, DES_BLOCK_SIZE);
    }
}

/* CBC mode decryption */
void des_cbc_decrypt(const DES_Context *ctx, const uint8_t iv[DES_BLOCK_SIZE],
                     const uint8_t *input, uint8_t *output, size_t length) {
    uint8_t prev_block[DES_BLOCK_SIZE];
    uint8_t decrypted[DES_BLOCK_SIZE];

    memcpy(prev_block, iv, DES_BLOCK_SIZE);

    for (size_t i = 0; i < length; i += DES_BLOCK_SIZE) {
        /* Decrypt */
        des_decrypt_block(ctx, input + i, decrypted);

        /* XOR with previous ciphertext block (or IV for first block) */
        for (int j = 0; j < DES_BLOCK_SIZE; j++) {
            output[i + j] = decrypted[j] ^ prev_block[j];
        }

        /* Save ciphertext for next round */
        memcpy(prev_block, input + i, DES_BLOCK_SIZE);
    }
}
