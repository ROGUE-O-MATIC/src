# Implementazione DES (Data Encryption Standard)

Questa è un'implementazione completa dell'algoritmo DES in C puro, creata per scopi educativi e di compatibilità con sistemi legacy.

## Avviso di Sicurezza

**IMPORTANTE**: DES è considerato criptograficamente obsoleto e NON dovrebbe essere utilizzato per applicazioni che richiedono sicurezza moderna. DES è stato sostituito da AES (Advanced Encryption Standard).

Utilizzare questa implementazione solo per:
- Scopi educativi
- Compatibilità con sistemi legacy
- Studio della crittografia
- Test e ricerca

## Caratteristiche

- Implementazione completa dell'algoritmo DES secondo FIPS PUB 46-3
- Supporto per modalità ECB (Electronic Codebook)
- Supporto per modalità CBC (Cipher Block Chaining)
- Cifratura e decifratura di blocchi singoli
- Nessuna dipendenza esterna (C standard library only)

## Specifiche Tecniche

- **Dimensione blocco**: 64 bit (8 byte)
- **Dimensione chiave**: 56 bit effettivi (64 bit con parità = 8 byte)
- **Round**: 16
- **Modalità supportate**: ECB, CBC

## Struttura dei File

```
des.h           - File header con dichiarazioni di funzioni e strutture
des.c           - Implementazione completa dell'algoritmo DES
des_example.c   - Esempi di utilizzo con vari test
Makefile        - File per la compilazione automatica
```

## Compilazione

### Usando Make (consigliato)

```bash
make
```

### Compilazione manuale

```bash
gcc -Wall -O2 -c des.c
gcc -Wall -O2 -c des_example.c
gcc -o des_example des.o des_example.o
```

## Esecuzione

```bash
./des_example
```

oppure

```bash
make run
```

## Esempi di Utilizzo

### Cifratura di un singolo blocco

```c
#include "des.h"

DES_Context ctx;
uint8_t key[8] = {0x13, 0x34, 0x57, 0x79, 0x9B, 0xBC, 0xDF, 0xF1};
uint8_t plaintext[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
uint8_t ciphertext[8];

// Inizializza DES con la chiave
des_init(&ctx, key);

// Cifra il blocco
des_encrypt_block(&ctx, plaintext, ciphertext);

// Decifra il blocco
uint8_t decrypted[8];
des_decrypt_block(&ctx, ciphertext, decrypted);
```

### Modalità ECB (blocchi multipli)

```c
DES_Context ctx;
uint8_t key[8] = {...};
uint8_t plaintext[16] = {...};  // 2 blocchi
uint8_t ciphertext[16];

des_init(&ctx, key);
des_ecb_encrypt(&ctx, plaintext, ciphertext, 16);
des_ecb_decrypt(&ctx, ciphertext, plaintext, 16);
```

### Modalità CBC (più sicura)

```c
DES_Context ctx;
uint8_t key[8] = {...};
uint8_t iv[8] = {...};  // Initialization Vector
uint8_t plaintext[16] = {...};
uint8_t ciphertext[16];

des_init(&ctx, key);
des_cbc_encrypt(&ctx, iv, plaintext, ciphertext, 16);
des_cbc_decrypt(&ctx, iv, ciphertext, plaintext, 16);
```

## API Reference

### Funzioni Principali

#### `void des_init(DES_Context *ctx, const uint8_t key[8])`
Inizializza il contesto DES con una chiave a 64 bit.

#### `void des_encrypt_block(const DES_Context *ctx, const uint8_t plaintext[8], uint8_t ciphertext[8])`
Cifra un singolo blocco da 64 bit.

#### `void des_decrypt_block(const DES_Context *ctx, const uint8_t ciphertext[8], uint8_t plaintext[8])`
Decifra un singolo blocco da 64 bit.

#### `void des_ecb_encrypt(const DES_Context *ctx, const uint8_t *input, uint8_t *output, size_t length)`
Cifra dati in modalità ECB. La lunghezza deve essere multipla di 8.

#### `void des_ecb_decrypt(const DES_Context *ctx, const uint8_t *input, uint8_t *output, size_t length)`
Decifra dati in modalità ECB.

#### `void des_cbc_encrypt(const DES_Context *ctx, const uint8_t iv[8], const uint8_t *input, uint8_t *output, size_t length)`
Cifra dati in modalità CBC con vettore di inizializzazione.

#### `void des_cbc_decrypt(const DES_Context *ctx, const uint8_t iv[8], const uint8_t *input, uint8_t *output, size_t length)`
Decifra dati in modalità CBC.

## Differenze tra ECB e CBC

### ECB (Electronic Codebook)
- Ogni blocco viene cifrato indipendentemente
- Stesso plaintext produce sempre stesso ciphertext
- **NON raccomandato** per la maggior parte degli usi (pattern visibili)
- Più veloce e parallelizzabile

### CBC (Cipher Block Chaining)
- Ogni blocco viene XORato con il blocco cifrato precedente
- Richiede un vettore di inizializzazione (IV)
- **Più sicuro** di ECB
- Non parallelizzabile in cifratura (ma sì in decifratura)

## Come Funziona DES

DES utilizza una rete di Feistel con 16 round:

1. **Permutazione iniziale (IP)**: Permuta i bit del blocco di input
2. **16 Round di Feistel**: Ogni round usa:
   - Funzione F con S-box e permutazioni
   - Sottochiave specifica del round
3. **Permutazione finale (FP)**: Inverso della permutazione iniziale

### Generazione delle Sottochiavi

La chiave a 64 bit viene ridotta a 56 bit (rimuovendo bit di parità), poi:
1. Permutata con PC-1
2. Divisa in due metà da 28 bit (C e D)
3. Per ogni round: rotazione circolare a sinistra e permutazione PC-2

## Limitazioni Note

- DES usa chiavi da solo 56 bit (facilmente violabili con forza bruta)
- Non resistente ad attacchi moderni
- Sostituito da 3DES e successivamente da AES
- Questa implementazione non è ottimizzata per prestazioni massime

## Triple DES (3DES)

Per maggiore sicurezza, considera l'implementazione di 3DES che applica DES tre volte:
```
Ciphertext = DES_encrypt(k3, DES_decrypt(k2, DES_encrypt(k1, Plaintext)))
```

## Riferimenti

- FIPS PUB 46-3: Data Encryption Standard (DES)
- NIST Special Publication 800-67: Recommendation for the Triple Data Encryption Algorithm (TDEA) Block Cipher

## Licenza

Questo codice è fornito per scopi educativi. Utilizzare a proprio rischio.

## Autore

Implementazione creata per dimostrare il funzionamento dell'algoritmo DES.
