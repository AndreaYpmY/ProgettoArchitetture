#!/bin/bash

# Dichiarazione delle variabili di base
FILE_C="prova"
FILE="prova"

# Lista di file NASM
FILES_NASM=(
    "prodotto_scalare" 
    "distanza_euclidea"
    "norma"
    "prodotto_matriciale"
)  # Aggiungi altri file .nasm qui

# Compilazione dei file NASM in formato elf32
for FILE_NASM in "${FILES_NASM[@]}"; do
    nasm -f elf32 ${FILE_NASM}.nasm -o ${FILE_NASM}.o
done

sleep 1
# Compilazione con GCC in modalità 32-bit e ottimizzazioni, 
# ATTENZIONE: includo tutti i file .o nella cartella corrente
gcc -m32 -msse -O0 -no-pie *.o ${FILE_C}.c -o ${FILE} -lm

# Esecuzione del file generato
./${FILE}