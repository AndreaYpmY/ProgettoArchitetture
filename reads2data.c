#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <xmmintrin.h>

#define	type		double
#define	MATRIX		type*
#define	VECTOR		type*

#define random() (((type) rand())/RAND_MAX)




void* get_block(int size, int elements) { 
	return _mm_malloc(elements*size,16); 
}

void free_block(void* p) { 
	_mm_free(p);
}

MATRIX alloc_matrix(int rows, int cols) {
	return (MATRIX) get_block(sizeof(type),rows*cols);
}

int* alloc_int_matrix(int rows, int cols) {
	return (int*) get_block(sizeof(int),rows*cols);
}

char* alloc_char_matrix(int rows, int cols) {
	return (char*) get_block(sizeof(char),rows*cols);
}

void dealloc_matrix(void* mat) {
	free_block(mat);
}


MATRIX load_data(char* filename, int *n, int *k) {
	FILE* fp;
	int rows, cols, status, i;
	
	fp = fopen(filename, "rb");
	
	if (fp == NULL){
		printf("'%s': bad data file name!\n", filename);
		exit(0);
	}
	
	status = fread(&cols, sizeof(int), 1, fp);
	status = fread(&rows, sizeof(int), 1, fp);
	
	MATRIX data = alloc_matrix(rows,cols);
	status = fread(data, sizeof(type), rows*cols, fp);
	fclose(fp);
	
	*n = rows;
	*k = cols;
	
	return data;
}
void save_to_txt(const char* filename, MATRIX data, int rows, int cols) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Errore nell'apertura del file '%s' per la scrittura!\n", filename);
        exit(1);
    }

    // Scrive la matrice nel file
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.5f ", data[i * cols + j]);  // Stampa con 5 cifre decimali
        }
        fprintf(fp, "\n");  // Nuova riga dopo ogni riga della matrice
    }

    fclose(fp);
    printf("Dati salvati con successo nel file '%s'.\n", filename);
}

int main(int argc, char** argv){
    int n=256;
    int k=1;
	

    MATRIX data = load_data(argv[1], &n, &k);
    
	/* Stampare a video la matrice 
    for(int i=0; i<n; i++){
        printf("%f ", data[i]);
        printf("\n");
    }*/
	save_to_txt(argv[2], data, n, k);

	//dealloco la memoria
	dealloc_matrix(data);
    
    return 0;
}

