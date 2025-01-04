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
extern type norma(VECTOR v);

/* Prodotto matriciale */
void prod_mat(VECTOR a, MATRIX b, VECTOR ris, int n){
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
//necessario: column-major order
void prod_mat_v2(VECTOR a, MATRIX b, VECTOR ris, int n){
    for (int i=0; i<n; i++){
        ris[i]=0;
        for (int j=0; j<n; j++){
            ris[i]+=a[j]*b[i*n+j];
        }
    }
}
extern type* prodmat(VECTOR a, MATRIX b);
extern type* prodmat_v2(VECTOR a, MATRIX b);





void print_matrix(type* m, int rows, int cols) {
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            printf("%f ", m[i*cols+j]);
        }
        printf("\n");
    }
}

int main(){
    clock_t t;
    type time;
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

    // per prodotto matriciale
    VECTOR prod_a=alloc_matrix(3,1);
    MATRIX prod_b=alloc_matrix(3,3);
    prod_a[0]=7.094;
    prod_a[1]=-5.2;
    prod_a[2]=3.1;
    prod_b[0]=1.0234;
    prod_b[1]=3.28999;
    prod_b[2]=8.44333;
    prod_b[3]=13.1;
    prod_b[4]=5.043;
    prod_b[5]=2.4555;
    prod_b[6]=10.032;
    prod_b[7]=2.3334;
    prod_b[8]=2.9323;

    // column-major order di prod_b

    MATRIX prod_c=alloc_matrix(3,3);
    prod_c[0]=1.0234;
    prod_c[1]=13.1;
    prod_c[2]=10.032;
    prod_c[3]=3.28999;
    prod_c[4]=5.043;
    prod_c[5]=2.3334;
    prod_c[6]=8.44333;
    prod_c[7]=2.4555;
    prod_c[8]=2.9323;

    VECTOR results_c=alloc_matrix(3,1);
    VECTOR results_v2=alloc_matrix(3,1);
    VECTOR results_asm;
    VECTOR results_asm_v2;

    t = clock();
    prod_mat(prod_a,prod_b,results_c,3);
    t = clock() - t;
    time = ((float)t)/CLOCKS_PER_SEC;
    printf("[time] Prodotto matriciale (C) = %.5f secs\n", time);

    t = clock();
    prod_mat_v2(prod_a,prod_c,results_v2,3);
    t = clock() - t;
    time = ((float)t)/CLOCKS_PER_SEC;
    printf("[time] Prodotto matriciale v2 (C) = %.5f secs\n", time);
    
    t = clock();
    results_asm=prodmat(prod_a,prod_b);
    t = clock() - t;
    time = ((float)t)/CLOCKS_PER_SEC;
    printf("[time] Prodotto matriciale (nasm) = %.5f secs\n", time);

    t = clock();
    results_asm_v2=prodmat_v2(prod_a,prod_c);
    t = clock() - t;
    time = ((float)t)/CLOCKS_PER_SEC;
    printf("[time] Prodotto matriciale v2 (nasm) = %.5f secs\n", time);

    printf("Prodotto scalare: %f (funzione C: %f)\n",ris,p(a,b,4));
    printf("Distanza euclidea: %f (funzione C: %f)\n",distanza,distanza_euclidea(a,b,4));
    printf("Norma: %f (funzione C: %f)\n",n,norma_c(a,4));
    
    printf("Prodotto matriciale (funzione C): \n");
    print_matrix(results_c,3,1);
    printf("Prodotto matriciale (funzione ASM): \n");
    print_matrix(results_asm,3,1);
    printf("Prodotto matriciale (funzione C v2): \n");
    print_matrix(results_v2,3,1);
    printf("Prodotto matriciale (funzione ASM v2): \n");
    print_matrix(results_asm_v2,3,1);

}
