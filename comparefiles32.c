#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compare_files_and_save_to_file(const char* file1, const char* phi, const char* psi, const char* output) {
    FILE *fp1, *fp2, *fp3, *fpout;
    float num1, num2;
    char line[512];
    int righe_phi = 0; // Contatore per le righe phi
    int righe_psi = 0; // Contatore per le righe psi
    int diff_phi = 0; // Contatore per le righe differenti phi
    int diff_psi = 0; // Contatore per le righe differenti psi

    // Apri i file
    fp1 = fopen(file1, "r");
    if (fp1 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", file1);
        exit(1);
    }

    fp2 = fopen(phi, "r");
    if (fp2 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", phi);
        fclose(fp1);
        exit(1);
    }

    fp3 = fopen(psi, "r");
    if (fp3 == NULL) {
        printf("Errore nell'apertura del file '%s'!\n", psi);
        fclose(fp1);
        fclose(fp2);
        exit(1);
    }

    fpout = fopen(output, "w");
    if (fpout == NULL) {
        printf("Errore nell'apertura del file '%s' per la scrittura!\n", output);
        fclose(fp1);
        fclose(fp2);
        fclose(fp3);
        exit(1);
    }

    // Confronta le righe
    fprintf(fpout, "Confronto delle righe:\n");

    while (fgets(line, sizeof(line), fp1) != NULL) {
        sscanf(line, "%f", &num1); // Leggi il numero dal primo file


        if (fgets(line, sizeof(line), fp2) != NULL) {

            sscanf(line, "%f", &num2); // Leggi il numero dal secondo file
            righe_phi += 1;

            if (num1 == num2) {
                fprintf(fpout, "[PHI]Uguale: %.6f\n", num1);
            } else {
                fprintf(fpout, "[PHI]Divergente: %.6f != %.6f\n", num1, num2);
                diff_phi += 1;
            }
        }else if(fgets(line, sizeof(line), fp3) != NULL){
            sscanf(line, "%f", &num2); // Leggi il numero dal secondo file
            righe_psi += 1;

            if (num1 == num2) {
                fprintf(fpout, "[PSI]Uguale: %.6f\n", num1);
            } else {
                fprintf(fpout, "[PSI]Divergente: %.6f != %.6f\n", num1, num2);
                diff_psi += 1;
            }
        }
    }

    fprintf(fpout,"Righe differenti PHI: %d/%d (Uguali: %d)\n", diff_phi, righe_phi, righe_phi - diff_phi);
    fprintf(fpout,"Righe differenti PSI: %d/%d (Uguali: %d)\n", diff_psi, righe_psi, righe_psi - diff_psi);


    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fpout);

    printf("Confronto completato. Risultati salvati in '%s'.\n", output);

}

// Funzione per confrontare due file
void compare_files(const char* file1, const char* file2) {
    FILE *fp1, *fp2;
    float num1, num2;
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
    
    int k1 = 0;
    int k2 = 0;

    float* ds_two = (float*) malloc(256 * sizeof(float));
    
    while(fgets(line, sizeof(line), fp1) != NULL) {
        sscanf(line, "%f", &num1);
        ds_two[k1] = num1;    
        k1++;
    }
    k1 = 0;

    // Confronta le righe
    printf("Confronto delle righe:\n");
    while (k2<512) {
        fgets(line, sizeof(line), fp2);
        k2++;
        sscanf(line, "%f", &num2); // Leggi il numero dal primo file

        if (k1<256) {
            if(k1==255)
                k1=0;
            else
                k1++;
            
            righe += 1;

            if (ds_two[k1]== num2) {
                printf("Uguale: %.6f\n", num2);
            } else {
                printf("Divergente: %.6f != %.6f\n", num2, ds_two[k1]);
                diff += 1;
            }
        }
    }

    fclose(fp1);
    fclose(fp2);

    printf("Righe differenti: %d/%d (Uguali: %d)\n", diff, righe, righe - diff);
}

int main(int argc, char** argv) {
    if (argc < 5) {
        printf("Uso: %s <file1.txt> <phi.txt> <psi.txt> <output.txt>\n", argv[0]);
        return 1;
    }

    //compare_files(argv[1], argv[2], argv[3], argv[4]);
    compare_files_and_save_to_file(argv[1], argv[2], argv[3],argv[4]);

    return 0;
}
