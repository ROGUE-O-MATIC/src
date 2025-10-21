# Makefile per la compilazione del progetto DES

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS =

# Target principali
all: des_example

# Compilazione dell'esempio DES
des_example: des_example.o des.o
	$(CC) $(CFLAGS) -o des_example des_example.o des.o $(LDFLAGS)

# Compilazione di des.o
des.o: des.c des.h
	$(CC) $(CFLAGS) -c des.c

# Compilazione di des_example.o
des_example.o: des_example.c des.h
	$(CC) $(CFLAGS) -c des_example.c

# Esegui l'esempio
run: des_example
	./des_example

# Pulisci i file compilati
clean:
	rm -f *.o des_example

# Compilazione javakill (codice originale)
javakill: javakill.c
	$(CC) $(CFLAGS) -DINETD javakill.c -o javakill

.PHONY: all run clean
