#!/bin/bash

# Questo script esegue i file C e confronta i risultati ottenuti

# Compila i file C
echo "Compilazione dei file C..."
gcc ./versione-c/pst.c -o ./versione-c/pst -lm
gcc reads2data.c -o read_file_ds2 -lm
gcc comparefiles.c -o confronto -lm

echo "Attesa di 2 secondi..."
sleep 2

# Variabili per i nomi degli eseguibili
pst_executable="versione-c/pst"
read_file_ds2_executable="./read_file_ds2"
confronto_executable="./confronto" 
# Nello script c di confronto in base alla funzione puoi scegliere se salvare il risultato su file o stamparlo a video

# Parametri per l'esecuzione dello script pst
seq_file="seq_256.ds2" # o ../seq_256.ds2
to_value=20
alpha_value=1
k_value=1
sd_value=3

# Parametri per l'esecuzione dello script reads2ds2
phi_file="phi_256_to20_k1_alpha1_sd3.ds2"

# Parametri per il confronto tra i file
ds2_To_txt="ds2.txt"
versione_c_out="pst.txt"

# Esegue il primo script
echo "Esecuzione del script pst..."
$pst_executable -seq $seq_file -to $to_value -alpha $alpha_value -k $k_value -sd $sd_value -d

# Genera file txt da ds2
echo "Generazione dei file txt da ds2..."
$read_file_ds2_executable $phi_file

echo "Attesa di 10 secondi..."
sleep 10

# Confronta i file
echo "Confronto tra i file..."
$confronto_executable $ds2_To_txt $versione_c_out
