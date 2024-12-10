#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compare_files_and_save_to_file(const char* file1, const char* file2, const char* output) {
    FILE *fp1, *fp2, *fpout;
    double num1, num2;
    char line[256];
    int righe = 0; // Contatore per le righe
    int diff = 0; // Contatore per le righe differenti

    // Apri i file
    fp1 = fopen(file1, "r");
    if (fp1 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", file1);
        exit(1);
    }

    fp2 = fopen(file2, "r");
    if (fp2 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", file2);
        fclose(fp1);
        exit(1);
    }

    fpout = fopen(output, "w");
    if (fpout == NULL) {
        printf("Errore nell'apertura del file '%s' per la scrittura!\n", output);
        fclose(fp1);
        fclose(fp2);
        exit(1);
    }

    // Confronta le righe
    fprintf(fpout, "Confronto delle righe:\n");
    while (fgets(line, sizeof(line), fp1) != NULL) {
        sscanf(line, "%lf", &num1); // Leggi il numero dal primo file

        if (fgets(line, sizeof(line), fp2) != NULL) {

            sscanf(line, "%lf", &num2); // Leggi il numero dal secondo file
            righe += 1;

            if (num1 == num2) {
                fprintf(fpout, "Uguale: %.5f\n", num1);
            } else {
                fprintf(fpout, "Divergente: %.5f != %.5f\n", num1, num2);
                diff += 1;
            }
        }
    }

    fprintf(fpout, "Righe differenti: %d/%d\n", diff, righe);

    fclose(fp1);
    fclose(fp2);
    fclose(fpout);

    printf("Confronto completato. Risultati salvati in '%s'.\n", output);

}

// Funzione per confrontare due file
void compare_files(const char* file1, const char* file2) {
    FILE *fp1, *fp2;
    double num1, num2;
    char line[256];
    int righe = 0; // Contatore per le righe
    int diff = 0; // Contatore per le righe differenti

    // Apri i file
    fp1 = fopen(file1, "r");
    if (fp1 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", file1);
        exit(1);
    }

    fp2 = fopen(file2, "r");
    if (fp2 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", file2);
        fclose(fp1);
        exit(1);
    }

    // Confronta le righe
    printf("Confronto delle righe:\n");
    while (fgets(line, sizeof(line), fp1) != NULL) {
        sscanf(line, "%lf", &num1); // Leggi il numero dal primo file

        if (fgets(line, sizeof(line), fp2) != NULL) {

            sscanf(line, "%lf", &num2); // Leggi il numero dal secondo file
            righe += 1;

            if (num1 == num2) {
                printf("Uguale: %.5f\n", num1);
            } else {
                printf("Divergente: %.5f != %.5f\n", num1, num2);
                diff += 1;
            }
        }
    }

    fclose(fp1);
    fclose(fp2);

    printf("Righe differenti: %d/%d\n", diff, righe);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Uso: %s <file1.txt> <file2.txt>\n", argv[0]);
        return 1;
    }

    //compare_files(argv[1], argv[2]);
    compare_files_and_save_to_file(argv[1], argv[2], "compare.txt");

    return 0;
}
