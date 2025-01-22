#!/bin/bash

# Questo script esegue i file C e confronta i risultati ottenuti

# Creazione di una nuova directory
run_dir="run"
mkdir -p $run_dir

# Creazione di una directory per i file di output all'interno di run
output_dir="$run_dir/output32"
mkdir -p $output_dir

# Compila i file C
echo "Compilazione dei file C..."
gcc -m32 -O0 ./versione-c/pst32c.c -o $run_dir/pst32 -lm
gcc reads2data32.c -o $run_dir/read_file_ds232 -lm
gcc comparefiles32.c -o $run_dir/confronto32 -lm

echo "Attesa di 2 secondi..."
sleep 2

# Variabili per i nomi degli eseguibili
pst_executable="$run_dir/pst32"
read_file_ds2_executable="$run_dir/read_file_ds232"
confronto_executable="$run_dir/confronto32" 

# Nello script c di confronto in base alla funzione puoi scegliere se salvare il risultato su file o stamparlo a video

# Parametri per l'esecuzione dello script pst
seq_file="seq_256.ds2" # o ../seq_256.ds2
to_value=20
alpha_value=1
k_value=1
sd_value=3

# Parametri per l'esecuzione dello script reads2ds2
phi_file="./32bit/phi_float.ds2"
psi_file="./32bit/psi_float.ds2"

# Parametri per il confronto tra i file
ds2_phi_txt="$output_dir/ds2_phi_To_txt32.txt"
ds2_psi_txt="$output_dir/ds2_psi_To_txt32.txt"
versione_c_out="$output_dir/pst32.txt"
confronto_out="$output_dir/confronto_out32.txt"


# Esegue il primo script
echo "Esecuzione del script pst..."
$pst_executable -seq $seq_file -to $to_value -alpha $alpha_value -k $k_value -sd $sd_value -d

# Genera file txt da ds2
echo "Generazione dei file txt da ds2..."
$read_file_ds2_executable $phi_file $ds2_phi_txt
$read_file_ds2_executable $psi_file $ds2_psi_txt

echo "Attesa di 3 secondi..."
sleep 3

# Confronta i file
echo "Confronto tra i file..."
$confronto_executable $versione_c_out $ds2_phi_txt $ds2_psi_txt $confronto_out
