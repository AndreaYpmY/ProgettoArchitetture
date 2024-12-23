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


/* Prodotto scalare */
type p(type *a, type *b, int n){
	type ris=0.0;
	for(int i=0; i<n;i++){
		ris+=a[i]*b[i];
	}
	return ris;
}
extern type prodotto(VECTOR a, VECTOR b);


/* Distanza euclidea */
type distanza_euclidea(VECTOR a, VECTOR b, int n){
	type ris=0.0;
	for(int i=0; i<n; i++){
		ris+=pow((b[i]-a[i]),2);
	}
	return sqrt(ris);
}
extern type dist(VECTOR a, VECTOR b);


/* Norma */
type norma_c(type* v, int n){
	type ris=0.0;
	for(int i=0; i<n; i++){
		ris+=v[i]*v[i];
	}
	return sqrt(ris);
}
type norma(VECTOR v);

/* Prodotto matriciale */
// moltiplicazione matriciale
void prod_mat(type* a, MATRIX b, type* ris, int n){
    int index=0;
	int k=0;
    for(int i=0; i<n; i++){
        ris[i]=0;
		index=0;
        for(int j=0; j<n; j++){
            ris[i]+=a[j]*b[j+index+k];
            index+=2;
        }
		k++;
    }
}
extern type prodmat(VECTOR a, MATRIX b, int n);




void print_matrix(MATRIX m, int rows, int cols) {
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            printf("%f ", m[i*cols+j]);
        }
        printf("\n");
    }
}

int main(){

    MATRIX a = alloc_matrix(4,1);
    MATRIX b = alloc_matrix(4,1);
    /*a[0]=0.323;
    a[1]=2.323;
    a[2]=-1.023;
    a[3]=-0.123;

    b[0]=0.323;
    b[1]=2.323;
    b[2]=-1.023;
    b[3]=-0.123;*/
    a[0] = 0.323;
    a[1] = 2.323;
    a[2] = -1.023;
    a[3] = -0.123;

    b[0] = 0.423;  
    b[1] = 2.423;  
    b[2] = -1.123; 
    b[3] = -0.223; 
    
    type ris = prodotto(a,b);

    type distanza = dist(a,b);

    type n = norma(a);

    //MATRIX prod_c= alloc_matrix(4,4);
    //prod_mat(a,b,prod_c,4);

    //MATRIX prod_asm= alloc_matrix(4,4);
    //prod_asm=prodmat(a,b,4);
    
    printf("Prodotto scalare: %f (funzione C: %f)\n",ris,p(a,b,4));
    printf("Distanza euclidea: %f (funzione C: %f)\n",distanza,distanza_euclidea(a,b,4));
    printf("Norma: %f (funzione C: %f)\n",n,norma_c(a,4));
    //printf("Prodotto matriciale: \n");
    //print_matrix(prod_c,4,4);
    //printf("Prodotto matriciale (funzione C): \n");
    //print_matrix(prod_asm,4,4);

}
