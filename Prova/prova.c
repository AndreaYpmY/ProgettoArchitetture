#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <xmmintrin.h>

#define	type		float
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


extern type prodotto(VECTOR a, VECTOR b);


int main(){

    MATRIX a = alloc_matrix(4,1);
    MATRIX b = alloc_matrix(4,1);
    a[0]=0.323;
    a[1]=2.323;
    a[2]=-1.023;
    a[3]=-0.123;

    b[0]=0.323;
    b[1]=2.323;
    b[2]=-1.023;
    b[3]=-0.123;
 
    type ris = prodotto(a,b);
    
    printf("Risultato: %f \n", ris);

}
