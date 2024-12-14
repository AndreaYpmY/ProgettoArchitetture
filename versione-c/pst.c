/**************************************************************************************
* 
* CdL Magistrale in Ingegneria Informatica
* Corso di Architetture e Programmazione dei Sistemi di Elaborazione - a.a. 2024/25
* 
* Progetto dell'algoritmo Predizione struttura terziaria proteine 221 231 a
* in linguaggio assembly x86-32 + SSE
* 
* F. Angiulli F. Fassetti S. Nisticò, novembre 2024
* 
**************************************************************************************/

/*
* 
* Software necessario per l'esecuzione:
* 
*    NASM (www.nasm.us)
*    GCC (gcc.gnu.org)
* 
* entrambi sono disponibili come pacchetti software 
* installabili mediante il packaging tool del sistema 
* operativo; per esempio, su Ubuntu, mediante i comandi:
* 
*    sudo apt-get install nasm
*    sudo apt-get install gcc
* 
* potrebbe essere necessario installare le seguenti librerie:
* 
*    sudo apt-get install lib32gcc-4.8-dev (o altra versione)
*    sudo apt-get install libc6-dev-i386
* 
* Per generare il file eseguibile:
* 
* nasm -f elf32 pst32.nasm && gcc -m32 -msse -O0 -no-pie sseutils32.o pst32.o pst32c.c -o pst32c -lm && ./pst32c $pars
* 
* oppure
* 
* ./runpst32
* 
*/

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

type hydrophobicity[] = {1.8, -1, 2.5, -3.5, -3.5, 2.8, -0.4, -3.2, 4.5, -1, -3.9, 3.8, 1.9, -3.5, -1, -1.6, -3.5, -4.5, -0.8, -0.7, -1, 4.2, -0.9, -1, -1.3, -1};		// hydrophobicity
type volume[] = {88.6, -1, 108.5, 111.1, 138.4, 189.9, 60.1, 153.2, 166.7, -1, 168.6, 166.7, 162.9, 114.1, -1, 112.7, 143.8, 173.4, 89.0, 116.1, -1, 140.0, 227.8, -1, 193.6, -1};		// volume
type charge[] = {0, -1, 0, -1, -1, 0, 0, 0.5, 0, -1, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, -1};		// charge

typedef struct {
	char* seq;		// sequenza di amminoacidi
	int N;			// lunghezza sequenza
	unsigned int sd; 	// seed per la generazione casuale
	type to;		// temperatura INIZIALE
	type alpha;		// tasso di raffredamento
	type k;		// costante
	VECTOR hydrophobicity; // hydrophobicity
	VECTOR volume;		// volume
	VECTOR charge;		// charge
	VECTOR phi;		// vettore angoli phi
	VECTOR psi;		// vettore angoli psi
	type e;		// energy
	int display;
	int silent;

} params;


/*
* 
*	Le funzioni sono state scritte assumento che le matrici siano memorizzate 
* 	mediante un array (float*), in modo da occupare un unico blocco
* 	di memoria, ma a scelta del candidato possono essere 
* 	memorizzate mediante array di array (float**).
* 
* 	In entrambi i casi il candidato dovr� inoltre scegliere se memorizzare le
* 	matrici per righe (row-major order) o per colonne (column major-order).
*
* 	L'assunzione corrente � che le matrici siano in row-major order.
* 
*/

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

/*
* 
* 	load_data
* 	=========
* 
*	Legge da file una matrice di N righe
* 	e M colonne e la memorizza in un array lineare in row-major order
* 
* 	Codifica del file:
* 	primi 4 byte: numero di righe (N) --> numero intero
* 	successivi 4 byte: numero di colonne (M) --> numero intero
* 	successivi N*M*4 byte: matrix data in row-major order --> numeri floating-point a precisione singola
* 
*****************************************************************************
*	Se lo si ritiene opportuno, � possibile cambiare la codifica in memoria
* 	della matrice. 
*****************************************************************************
* 
*/
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

/*
* 
* 	load_seq
* 	=========
* 
*	Legge da file una matrice di N righe
* 	e M colonne e la memorizza in un array lineare in row-major order
* 
* 	Codifica del file:
* 	primi 4 byte: numero di righe (N) --> numero intero
* 	successivi 4 byte: numero di colonne (M) --> numero intero
* 	successivi N*M*1 byte: matrix data in row-major order --> charatteri che compongono la stringa
* 
*****************************************************************************
*	Se lo si ritiene opportuno, � possibile cambiare la codifica in memoria
* 	della matrice. 
*****************************************************************************
* 
*/
char* load_seq(char* filename, int *n, int *k) {
	FILE* fp;
	int rows, cols, status, i;
	
	fp = fopen(filename, "rb");
	
	if (fp == NULL){
		printf("'%s': bad data file name!\n", filename);
		exit(0);
	}
	
	status = fread(&cols, sizeof(int), 1, fp);
	status = fread(&rows, sizeof(int), 1, fp);

	
	char* data = alloc_char_matrix(rows,cols);
	status = fread(data, sizeof(char), rows*cols, fp);
	fclose(fp);
	
	*n = rows;
	*k = cols;
	
	return data;
}

/*
* 	save_data
* 	=========
* 
*	Salva su file un array lineare in row-major order
*	come matrice di N righe e M colonne
* 
* 	Codifica del file:
* 	primi 4 byte: numero di righe (N) --> numero intero a 32 bit
* 	successivi 4 byte: numero di colonne (M) --> numero intero a 32 bit
* 	successivi N*M*4 byte: matrix data in row-major order --> numeri interi o floating-point a precisione singola
*/
void save_data(char* filename, void* X, int n, int k) {
	FILE* fp;
	int i;
	fp = fopen(filename, "wb");
	if(X != NULL){
		fwrite(&k, 4, 1, fp);
		fwrite(&n, 4, 1, fp);
		for (i = 0; i < n; i++) {
			fwrite(X, sizeof(type), k, fp);
			//printf("%i %i\n", ((int*)X)[0], ((int*)X)[1]);
			X += sizeof(type)*k;
		}
	}
	else{
		int x = 0;
		fwrite(&x, 4, 1, fp);
		fwrite(&x, 4, 1, fp);
	}
	fclose(fp);
}

/*
* 	save_out
* 	=========
* 
*	Salva su file un array lineare composto da k elementi.
* 
* 	Codifica del file:
* 	primi 4 byte: contenenti l'intero 1 		--> numero intero a 32 bit
* 	successivi 4 byte: numero di elementi k     --> numero intero a 32 bit
* 	successivi byte: elementi del vettore 		--> k numero floating-point a precisione singola
*/
void save_out(char* filename, MATRIX X, int k) {
	FILE* fp;
	int i;
	int n = 1;
	fp = fopen(filename, "wb");
	if(X != NULL){
		fwrite(&n, 4, 1, fp);
		fwrite(&k, 4, 1, fp);
		fwrite(X, sizeof(type), k, fp);
	}
	fclose(fp);
}

/*
* 	gen_rnd_mat
* 	=========
* 
*	Genera in maniera casuale numeri reali tra -pi e pi
*	per riempire una struttura dati di dimensione Nx1
* 
*/
void gen_rnd_mat(VECTOR v, int N){
	int i;

	for(i=0; i<N; i++){
		// Campionamento del valore + scalatura
		v[i] = (random()*2 * M_PI) - M_PI;
	}
}

// PROCEDURE ASSEMBLY
extern void prova(params* input);

//prodotto scalare
type p(type *a, type *b, int n){
	type ris=0.0;
	for(int i=0; i<n;i++){
		ris+=a[i]*b[i];
	}
	return ris;
}

type coseno(type x){
	return 1-((x*x)/2)+((x*x*x*x)/24)-((x*x*x*x*x*x)/720);
}
type seno(type x){
	return x-((x*x*x)/6)+((x*x*x*x*x)/120)-((x*x*x*x*x*x*x)/5040);
}

void rotation(VECTOR axis, type theta,  MATRIX matrix){
	const int n=3;
	type ps = p(axis, axis, n);
	for(int k=0; k<n; k++)
		axis[k] = axis[k]/ps;
	type a= coseno((theta/2.0));
	type b= -1*axis[0]*seno((theta/2.0));
	type c= -1*axis[1]*seno((theta/2.0));
	type d= -1*axis[2]*seno((theta/2.0));

	
	matrix[0]=a*a+b*b-c*c-d*d;
	matrix[1]=2*(b*c+a*d);
	matrix[2]=2*(b*d-a*c);
	matrix[3]=2*(b*c-a*d);
	matrix[4]=a*a+c*c-b*b-d*d;
	matrix[5]=2*(c*d+a*b);
	matrix[6]=2*(b*d+a*c);
	matrix[7]=2*(c*d-a*b);
	matrix[8]=a*a+d*d-b*b-c*c;
}

type norma(type* v, int n){
	type ris=0.0;
	for(int i=0; i<n; i++){
		ris+=v[i]*v[i];
	}
	return sqrt(ris);
}

// moltiplicazione matriciale
void prod_mat(type* a, MATRIX b, type* ris, int n){
    int index=0;
    for(int i=0; i<n; i++){
        ris[i]=0;
        for(int j=0; j<n; j++){
            ris[i]+=a[j]*b[index];
            index++;
        }
    }
}






// s: sequenza di amminoacidi
void backbone(char* s, VECTOR phi, VECTOR psi, MATRIX coords){
	int n = 256; // lunghezza sequenza
	type r_ca_n = 1.46; //distanza CA-N
	type r_ca_c = 1.52; //distanza CA-C
	type r_c_n = 1.33; //distanza C-N
	
	//Angoli standard del backbone
	type theta_ca_c_n=2.028;
	type theta_c_n_ca=2.124;
	type theta_n_ca_c=1.940;
	
	/*
		Struttura coords:
			coords[A][B][C] = ...
			A: indice amminoacido 
			B: indice atomo: 0=N, 1=C alpha, 2=C
			C: coordinata: 0=x, 1=y, 2=z

		    coords[A][B] = ...
			A: indice amminoacio (ogni 3) + indice atomo
			B: coordinata: 0=x, 1=y, 2=z
			pos. 0: 1° amminoacido
			pos. 3: 2° amminoacido

			coords[A] = ...
			A: indice amminoacio (ogni 9)  + indice atomo + coordinata 
			pos. 0: 1° amminoacido, atomo N, coordinata x
			pos. 1: 1° amminoacido, atomo N, coordinata y
			pos. 2: 1° amminoacido, atomo N, coordinata z
			pos. 3: 1° amminoacido, atomo C alpha, coordinata x
			- - -
			pos. 9: 2° amminoacido, atomo N, coordinata x
			pos. 10: 2° amminoacido, atomo N, coordinata y
	*/

	//N
	coords[0] = 0; 
	coords[1] = 0;
	coords[2] = 0;

	//C alpha
	coords[3] = r_ca_n;
	coords[4] = 0;
	coords[5] = 0;

	// Vettori e matrici utilizzati
	type* v1;
	type* v2;
	type* v3;
	MATRIX rot;
	type* newv;
	type* vettore_ausilio;
	
	v1 = alloc_matrix(3,1);
	v2 = alloc_matrix(3,1);
	v3 = alloc_matrix(3,1);
	rot = alloc_matrix(3,3);
	newv = alloc_matrix(3,1);
	vettore_ausilio = alloc_matrix(3,1);

	vettore_ausilio[0] = 0;
	vettore_ausilio[1] = 0;
	vettore_ausilio[2] = 0;

	
	for (int i = 0; i <n; i++){
		int idx= i*9;

		if(i>0){
			//		Posiziona N usando l'ultimo C
			
			// [i-1] indice amminoacido precedente, atomi C alpha e C, coordinate x,y,z
			v1[0] = coords[idx-3]-coords[idx-6]; 
			v1[1] = coords[idx-2]-coords[idx-5];
			v1[2] = coords[idx-1]-coords[idx-4];
			
			// calcola norma
			type norma_v1 = norma(v1, 3);
			for(int j=0; j<3; j++){
				v1[j] = v1[j]/norma_v1;
			}

			//rotazione
			rotation(v1, theta_c_n_ca, rot);

			//moltiplicazione matriciale
			vettore_ausilio[1] = r_c_n;
			prod_mat(vettore_ausilio, rot, newv, 3);

			//posiziona N con le coordinate calcolate
			coords[idx] = coords[idx-3]+newv[0];
			coords[idx+1] = coords[idx-2]+newv[1];
			coords[idx+2] = coords[idx-1]+newv[2];


			//		Posiziona C alpha usando phi

			// [i] indice amminoacido corrente, atomi N e C alpha, coordinate x,y,z
			v2[0] = coords[idx]-coords[idx-3];
			v2[1] = coords[idx+1]-coords[idx-2];
			v2[2] = coords[idx+2]-coords[idx-1];

			// calcola norma
			type norma_v2 = norma(v2, 3);
			for(int j=0; j<3; j++){
				v2[j] = v2[j]/norma_v2;
			}

			//rotazione
			rotation(v2, phi[i], rot);

			//moltiplicazione matriciale
			vettore_ausilio[1] = r_ca_n;
			prod_mat(vettore_ausilio, rot, newv, 3);

			//posiziona C alpha con le coordinate calcolate
			coords[idx+3] = coords[idx]+newv[0];
			coords[idx+4] = coords[idx+1]+newv[1];
			coords[idx+5] = coords[idx+2]+newv[2];
		}

		//		Posiziona C usasndo psi

		// [i] indice amminoacido corrente, atomi N e C alpha, coordinate x,y,z
		v3[0] = coords[idx+3]-coords[idx];
		v3[1] = coords[idx+4]-coords[idx+1];
		v3[2] = coords[idx+5]-coords[idx+2];	

		// calcola norma
		type norma_v3 = norma(v3, 3);
		for(int j=0; j<3; j++){
			v3[j] = v3[j]/norma_v3;
		}

		//rotazione
		rotation(v3, psi[i], rot);



		//moltiplicazione matriciale
		vettore_ausilio[1] = r_ca_c;
		prod_mat(vettore_ausilio, rot, newv, 3);

		//posiziona C con le coordinate calcolate
		coords[idx+6] = coords[idx+3]+newv[0];
		coords[idx+7] = coords[idx+4]+newv[1];
		coords[idx+8] = coords[idx+5]+newv[2];
	}
}

type min(type a, type b){
	if(a<b)
		return a;
	return b;
}

type rama_energy(VECTOR phi, VECTOR psi, int n){
	type alpha_phi=-57.8;
	type alpha_psi=-47.0;
	type beta_phi=-119.0;
	type beta_psi=113.0;

	type energy=0.0;
	for(int i=0; i<n; i++){
		type alpha_dist=sqrt(pow((phi[i]-alpha_phi),2)+pow((psi[i]-alpha_psi),2));
		type beta_dist=sqrt(pow((phi[i]-beta_phi),2)+pow((psi[i]-beta_psi),2));
		type min_dist=min(alpha_dist, beta_dist);
		energy+=(0.5*min_dist);
	}

	return energy;
}

type distanza_euclidea(VECTOR a, VECTOR b, int n){
	type ris=0.0;
	for(int i=0; i<n; i++){
		//invertiti?
		ris+=pow((b[i]-a[i]),2);
	}
	return sqrt(ris);
}

type dist(VECTOR A1, VECTOR A2){
	return distanza_euclidea(A1, A2, 3);
}

VECTOR get_C_alpha(MATRIX coords, int index){
	VECTOR c_alpha;
	c_alpha = alloc_matrix(3,1); // x,y,z
	c_alpha[0] = coords[index+3];
	c_alpha[1] = coords[index+4];
	c_alpha[2] = coords[index+5];
	return c_alpha;
}

type hydrophobicity_energy(char* s, MATRIX coords, int n){
	type energy=0.0;

	VECTOR c_alpha_i;
	VECTOR c_alpha_j;

	c_alpha_i =alloc_matrix(3,1);
	c_alpha_j =alloc_matrix(3,1);


	for(int i=0; i < n; i+=9){
		for(int j=i+9; j<n; j+=9){
			
			c_alpha_i = get_C_alpha(coords, i);
			c_alpha_j = get_C_alpha(coords, j);
			type distanza=dist(c_alpha_i, c_alpha_j);
			if(distanza < 10.0){
				int index_i = s[i] - 'A';
				int index_j = s[j] - 'A';
				energy += (hydrophobicity[index_i]*hydrophobicity[index_j])/distanza;
			}
		}
	}
	return energy;
}

type electrostatic_energy(char* s, MATRIX coords, int n){
	type energy=0.0;

	VECTOR c_alpha_i;
	VECTOR c_alpha_j;

	c_alpha_i =alloc_matrix(3,1);
	c_alpha_j =alloc_matrix(3,1);

	for(int i=0; i<n;i+=9){
		for(int j=i+9; j<n; j+=9){
			c_alpha_i = get_C_alpha(coords, i);
			c_alpha_j = get_C_alpha(coords, j);
			type distanza=dist(c_alpha_i, c_alpha_j);

			int index_i = s[i] - 'A';
			int index_j = s[j] - 'A';

			if(i!=j && distanza < 10.0 && charge[index_i]!=0 && charge[index_j]!=0){
				energy += (charge[index_i]*charge[index_j])/(distanza*4.0);
			}
		}
	}

	return energy;
}

type packing_energy(char* s, MATRIX coords, int n){
	type energy=0.0;

	VECTOR c_alpha_i;
	VECTOR c_alpha_j;

	c_alpha_i =alloc_matrix(3,1);
	c_alpha_j =alloc_matrix(3,1);



	for(int i=0; i<n; i+=9){

		int index_i = s[i] - 'A';
		type density=0.0;

		for(int j=0; j<n; j+=9){

			int index_j = s[j] - 'A';
			c_alpha_i = get_C_alpha(coords, i);
			c_alpha_j = get_C_alpha(coords, j);
			type distanza=dist(c_alpha_i, c_alpha_j);

			if(i!=j && distanza < 10.0){
				density += (volume[index_j]/pow(distanza,3));
			}
		}
		energy += pow((volume[index_i]-density),2);
	}

	return energy;
}

type energy(char* s, VECTOR phi, VECTOR psi, int n){
	MATRIX coords;
	coords = alloc_matrix(n*3,3);

	backbone(s, phi, psi, coords);
	type total_energy=0.0;

	for(int i=0; i<25; i++){
		printf("Coords[%i]: %f \n ", i, coords[i]);
	}

	type rama_e =rama_energy(phi, psi, n);
	type hydro_e = hydrophobicity_energy(s, coords, n);
	type ele_e = electrostatic_energy(s, coords, n);
	type pack_e = packing_energy(s, coords, n);

	printf("Rama: %f, Hydro: %f, Ele: %f, Pack: %f \n", rama_e, hydro_e, ele_e, pack_e);

	type w_rama = 1.0;
	type w_hidro = 0.5;
	type w_elec = 0.2;
	type w_pack = 0.3;

	total_energy += rama_e*w_rama;
	total_energy += hydro_e*w_hidro;
	total_energy += ele_e*w_elec;
	total_energy += pack_e*w_pack;
	return total_energy;
}

type prob_accept(type delta_E, type k, type T){
	printf("Delta E: %f, k: %f, T: %f \n", delta_E, k, T);
	type ris= exp(-delta_E/(k*T));
	printf("ris: %f \n", ris);
	return ris;
}

void stampa_vettori(VECTOR phi, VECTOR psi, int n){
	printf("phi: ");
	for(int i=0; i<n; i++){
		printf("%f, ", phi[i]);
	}
	printf("\n psi: ");
	for(int i=0; i<n; i++){
		printf("%f, ", psi[i]);
	}
	printf("\n");
}

void confronta_vettori(VECTOR v1, VECTOR v2, int n){
	int c=0;
	int indice=-1;
	for(int i=0; i<n;i++){
		if(v1[i]!=v2[i]){
			c=1;
			indice=i;
			break;
		}
	}
	if(c==0)
		printf("Nessun cambio \n");
	else
		printf("Cambio in posizone: %d \n", indice);
}



void pst(params* input){
	int n = input->N;
	char* s = input->seq;
	type to = input->to;
	type T = to;
	type alpha = input->alpha;
	type k = input->k;
	VECTOR phi = input->phi;
	VECTOR psi = input->psi;
	type E = input->e;


	E = energy(s, phi, psi, n);
	exit (0);
	int t=0;
	
	while (T>0.0){
		int i=rand()%(n+1);
		printf("Numero random: %d \n", i);
		
		type theta_phi= (random()*2 * M_PI) - M_PI;
		
		//printf("Phi prima: %f \n", phi[i]);

		phi[i]=phi[i]+theta_phi;

		//printf("Phi dopo: %f \n", phi[i]);



		type theta_psi= (random()*2 * M_PI) - M_PI;
		psi[i]=psi[i]+theta_psi;

		type E_new = energy(s, phi, psi, n);
		type delta_E = E_new - E;

		//printf("Variazione energia: %f \n", delta_E);

		if(delta_E <= 0){

			//printf("Phi dopo variazione e: %f \n", phi[i]);

			E = E_new;
		}else{
			type P = prob_accept(delta_E, k, T);
			type r = random();

			printf("P: %f e ", P);
			printf("r: %f \n" , r);

			if(r <= P){

				//printf("Phi dopo r minore uguale P: %f \n", phi[i]);

				E = E_new;
			}else{
				phi[i]=phi[i]-theta_phi;
				psi[i]=psi[i]+theta_psi;

				//printf("Phi dopo non accettato: %f \n", phi[i]);
			}
		}
		t+=1;
		T = to-sqrt(alpha*t);
	}
	input->e = E;
	
	
	
}

void save_to_txt(const char* filename, MATRIX data1,MATRIX data2, int rows, int cols) {//TODO: Eliminare a fine test
 
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Errore nell'apertura del file '%s' per la scrittura!\n", filename);
        exit(1);
    }
 
    //fprintf(fp, "data1\n");
 
    // Scrive la matrice nel file
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.5f ", data1[i * cols + j]);  // Stampa con 5 cifre decimali
        }
        fprintf(fp, "\n");
    }
 
    //fprintf(fp,"data2\n");
 
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.5f ", data2[i * cols + j]);  // Stampa con 5 cifre decimali
        }
        fprintf(fp, "\n");
    }
 
    fclose(fp);
    printf("Dati salvati con successo nel file '%s'.\n", filename);
}

int main(int argc, char** argv) {
	char fname_phi[256];
	char fname_psi[256];
	char* seqfilename = NULL;
	clock_t t;
	float time;
	int d;
	
	//
	// Imposta i valori di default dei parametri
	//
	params* input = malloc(sizeof(params));
	input->seq = NULL;	
	input->N = -1;			
	input->to = -1;
	input->alpha = -1;
	input->k = -1;		
	input->sd = -1;		
	input->phi = NULL;		
	input->psi = NULL;
	input->silent = 0;
	input->display = 0;
	input->e = -1;
	input->hydrophobicity = hydrophobicity;
	input->volume = volume;
	input->charge = charge;


	//
	// Visualizza la sintassi del passaggio dei parametri da riga comandi
	//
	if(argc <= 1){
		printf("%s -seq <SEQ> -to <to> -alpha <alpha> -k <k> -sd <sd> [-s] [-d]\n", argv[0]);
		printf("\nParameters:\n");
		printf("\tSEQ: il nome del file ds2 contenente la sequenza amminoacidica\n");
		printf("\tto: parametro di temperatura\n");
		printf("\talpha: tasso di raffredamento\n");
		printf("\tk: costante\n");
		printf("\tsd: seed per la generazione casuale\n");
		printf("\nOptions:\n");
		printf("\t-s: modo silenzioso, nessuna stampa, default 0 - false\n");
		printf("\t-d: stampa a video i risultati, default 0 - false\n");
		exit(0);
	}

	//
	// Legge i valori dei parametri da riga comandi
	//

	int par = 1;
	while (par < argc) {
		if (strcmp(argv[par],"-s") == 0) {
			input->silent = 1;
			par++;
		} else if (strcmp(argv[par],"-d") == 0) {
			input->display = 1;
			par++;
		} else if (strcmp(argv[par],"-seq") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing dataset file name!\n");
				exit(1);
			}
			seqfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-to") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing to value!\n");
				exit(1);
			}
			input->to = atof(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-alpha") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing alpha value!\n");
				exit(1);
			}
			input->alpha = atof(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-k") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing k value!\n");
				exit(1);
			}
			input->k = atof(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-sd") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing seed value!\n");
				exit(1);
			}
			input->sd = atoi(argv[par]);
			par++;
		}else{
			printf("WARNING: unrecognized parameter '%s'!\n",argv[par]);
			par++;
		}
	}

	//
	// Legge i dati e verifica la correttezza dei parametri
	//
	if(seqfilename == NULL || strlen(seqfilename) == 0){
		printf("Missing ds file name!\n");
		exit(1);
	}

	input->seq = load_seq(seqfilename, &input->N, &d);

	
	if(d != 1){
		printf("Invalid size of sequence file, should be %ix1!\n", input->N);
		exit(1);
	} 

	if(input->to <= 0){
		printf("Invalid value of to parameter!\n");
		exit(1);
	}

	if(input->k <= 0){
		printf("Invalid value of k parameter!\n");
		exit(1);
	}

	if(input->alpha <= 0){
		printf("Invalid value of alpha parameter!\n");
		exit(1);
	}


	

	input->phi = alloc_matrix(input->N, 1);
	input->psi = alloc_matrix(input->N, 1);
	// Impostazione seed 
	srand(input->sd);
	// Inizializzazione dei valori
	gen_rnd_mat(input->phi, input->N);
	gen_rnd_mat(input->psi, input->N);

	//
	// Visualizza il valore dei parametri
	//

	if(!input->silent){
		printf("Dataset file name: '%s'\n", seqfilename);
		printf("Sequence lenght: %d\n", input->N);
	}

	// COMMENTARE QUESTA RIGA!
	//prova(input);
	//

	//
	// Predizione struttura terziaria
	//
	t = clock();
	pst(input);
	t = clock() - t;
	time = ((float)t)/CLOCKS_PER_SEC;
	save_to_txt("pst.txt", input->phi, input->psi, input->N, 1);//TODO: Eliminare a fine test

	if(!input->silent)
		printf("PST time = %.3f secs\n", time);
	else
		printf("%.3f\n", time);

	//
	// Salva il risultato
	//
	sprintf(fname_phi, "out32_%d_%d_phi.ds2", input->N, input->sd);
	save_out(fname_phi, input->phi, input->N);
	sprintf(fname_psi, "out32_%d_%d_psi.ds2", input->N, input->sd);
	save_out(fname_psi, input->psi, input->N);
	if(input->display){	
		if(input->phi == NULL || input->psi == NULL)
			printf("out: NULL\n");
		else{
			int i,j;
			printf("energy: %f, phi: [", input->e);
			for(i=0; i<input->N; i++){
				printf("%f,", input->phi[i]);
			}
			printf("]\n");
			printf("psi: [");
			for(i=0; i<input->N; i++){
				printf("%f,", input->psi[i]);
			}
			printf("]\n");
		}
	}

	if(!input->silent)
		printf("\nDone.\n");

	dealloc_matrix(input->phi);
	dealloc_matrix(input->psi);
	free(input);

	return 0;
}
