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
#include <omp.h>

#define	type		double
#define	MATRIX		type*
#define	VECTOR		type*

#define random() (((type) rand())/RAND_MAX)

type hydrophobicity[] = {1.8, -1, 2.5, -3.5, -3.5, 2.8, -0.4, -3.2, 4.5, -1, -3.9, 3.8, 1.9, -3.5, -1, -1.6, -3.5, -4.5, -0.8, -0.7, -1, 4.2, -0.9, -1, -1.3, -1};		// hydrophobicity
type volume[] = {88.6, -1, 108.5, 111.1, 138.4, 189.9, 60.1, 153.2, 166.7, -1, 168.6, 166.7, 162.9, 114.1, -1, 112.7, 143.8, 173.4, 89.0, 116.1, -1, 140.0, 227.8, -1, 193.6, -1};		// volume
type charge[] = {0, -1, 0, -1, -1, 0, 0, 0.5, 0, -1, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, -1};		// charge

type r_ca_n = 1.46; //distanza CA-N
type r_ca_c = 1.52; //distanza CA-C
type r_c_n = 1.33; //distanza C-N
	
//Angoli standard del backbone
type theta_ca_c_n=2.028;
type theta_c_n_ca=2.124;
type theta_n_ca_c=1.940;

type w_rama = 1.0;
type w_hidro = 0.5;
type w_elec = 0.2;
type w_pack = 0.3;

int number_of_threads=6;

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
	return _mm_malloc(elements*size,32); 
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
//extern void prova(params* input);
extern void dist(VECTOR a, VECTOR b, type* dist);
extern void prodmat(VECTOR a, MATRIX b, VECTOR results);
extern void norma(VECTOR v, VECTOR vn);
extern void rama(VECTOR phi, VECTOR psi, int n, type* rama_e);




//prodotto scalare
type prod_scalare(type *a, type *b, int n){
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
	type ps = prod_scalare(axis, axis, n);
	for(int k=0; k<n; k++)
		axis[k] = axis[k]/ps;
	type a= coseno((theta/2.0));
	type b= -1*axis[0]*seno((theta/2.0));
	type c= -1*axis[1]*seno((theta/2.0));
	type d= -1*axis[2]*seno((theta/2.0));


	matrix[0]=a*a+b*b-c*c-d*d;
	matrix[4]=2*(b*c+a*d);
	matrix[8]=2*(b*d-a*c);
	matrix[1]=2*(b*c-a*d);
	matrix[5]=a*a+c*c-b*b-d*d;
	matrix[9]=2*(c*d+a*b);
	matrix[2]=2*(b*d+a*c);
	matrix[6]=2*(c*d-a*b);
	matrix[10]=a*a+d*d-b*b-c*c;
	matrix[3]=0.0;
	matrix[7]=0.0;
	matrix[11]=0.0;
}


void backbone(char* s, VECTOR phi, VECTOR psi, MATRIX coords, int n){

	//N
	coords[0] = 0; 
	coords[1] = 0;
	coords[2] = 0;

	//C alpha
	coords[3] = r_ca_n;
	coords[4] = 0;
	coords[5] = 0;

	// Vettori e matrici utilizzati
	VECTOR v1;
	VECTOR v2;
	VECTOR v3;
	VECTOR v1n;
	VECTOR v2n;
	VECTOR v3n;
	MATRIX rot;
	VECTOR newv;
	VECTOR vettore_ausilio;
	
	v1 = alloc_matrix(4,1);
	v2 = alloc_matrix(4,1);
	v3 = alloc_matrix(4,1);
	v1n = alloc_matrix(4,1);
	v2n = alloc_matrix(4,1);
	v3n = alloc_matrix(4,1);

	rot = alloc_matrix(4,3);
	newv = alloc_matrix(4,1);
	vettore_ausilio = alloc_matrix(4,1);

	vettore_ausilio[0] = 0;
	vettore_ausilio[1] = 0;
	vettore_ausilio[2] = 0;
	vettore_ausilio[3] = 0;
	
	for (int i = 0; i <n; i++){
		int idx= i*9;

		if(i>0){
			//		Posiziona N usando l'ultimo C
			
			// [i-1] indice amminoacido precedente, atomi C alpha e C, coordinate x,y,z
			v1[0] = coords[idx-3]-coords[idx-6]; 
			v1[1] = coords[idx-2]-coords[idx-5];
			v1[2] = coords[idx-1]-coords[idx-4];
			v1[3] = 0;
			
			// calcola norma
			norma(v1, v1n);

			//rotazione
			rotation(v1n, theta_c_n_ca, rot);

			//moltiplicazione matriciale
			vettore_ausilio[1] = r_c_n;
			prodmat(vettore_ausilio, rot, newv);

			//posiziona N con le coordinate calcolate
			
			coords[idx] = coords[idx-3]+newv[0];
			coords[idx+1] = coords[idx-2]+newv[1];
			coords[idx+2] = coords[idx-1]+newv[2];
			

			//		Posiziona C alpha usando phi

			// [i] indice amminoacido corrente, atomi N e C alpha, coordinate x,y,z
			v2[0] = coords[idx]-coords[idx-3];
			v2[1] = coords[idx+1]-coords[idx-2];
			v2[2] = coords[idx+2]-coords[idx-1];
			v2[3] = 0;

			// calcola norma
			norma(v2, v2n);
		
			//rotazione
			rotation(v2n, phi[i], rot);

			//moltiplicazione matriciale
			vettore_ausilio[1] = r_ca_n;
			prodmat(vettore_ausilio, rot, newv);

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
		v3[3] = 0;

		// calcola norma
		norma(v3, v3n);

		//rotazione
		rotation(v3n, psi[i], rot);

		//moltiplicazione matriciale
		vettore_ausilio[1] = r_ca_c;
		prodmat(vettore_ausilio, rot, newv);

		//posiziona C con le coordinate calcolate
		coords[idx+6] = coords[idx+3]+newv[0];
		coords[idx+7] = coords[idx+4]+newv[1];
		coords[idx+8] = coords[idx+5]+newv[2];
		
	}

	dealloc_matrix(vettore_ausilio);
	dealloc_matrix(rot);
	dealloc_matrix(newv);
	dealloc_matrix(v1);
	dealloc_matrix(v2);
	dealloc_matrix(v3);
	dealloc_matrix(v1n);
	dealloc_matrix(v2n);
	dealloc_matrix(v3n);
}


VECTOR get_C_alpha(MATRIX coords, int n){
	VECTOR all_C_alpha;
	all_C_alpha = alloc_matrix(n*3, 1);

	#pragma omp parallel for schedule(dynamic)
	for(int i = 0; i< n; i++){
		all_C_alpha[i*3] = coords[(i*9)+3];
		all_C_alpha[(i*3)+1] = coords[(i*9)+4];
		all_C_alpha[(i*3)+2] = coords[(i*9)+5];
	}
	return all_C_alpha;
}


type hydrophobicity_energy(char* s, MATRIX coords, int n, VECTOR all_c_alpha){
	type energy=0.0;
	VECTOR c_alpha_i;
	VECTOR c_alpha_j;
	type distanza;

	
	#pragma omp parallel for private(c_alpha_i, c_alpha_j, distanza) reduction(+ : energy) schedule(dynamic)
	for(int i=0; i < n; i++){
		c_alpha_i =alloc_matrix(4,1);
		c_alpha_j =alloc_matrix(4,1);

		c_alpha_i[0] = all_c_alpha[i*3];
		c_alpha_i[1] = all_c_alpha[(i*3)+1];
		c_alpha_i[2] = all_c_alpha[(i*3)+2];
		c_alpha_i[3] = 0;

		for(int j=i+1; j<n; j++){
			c_alpha_j[0] = all_c_alpha[j*3];
			c_alpha_j[1] = all_c_alpha[(j*3)+1];
			c_alpha_j[2] = all_c_alpha[(j*3)+2];
			c_alpha_j[3] = 0;
	
			dist(c_alpha_i, c_alpha_j, &distanza);

			if(distanza < 10.0){
				int index_i = s[i] - 'A';
				int index_j = s[j] - 'A';
				energy += (hydrophobicity[index_i]*hydrophobicity[index_j])/distanza;
			}
		}
		dealloc_matrix(c_alpha_i);
		dealloc_matrix(c_alpha_j);
	}
	return energy;
}


type electrostatic_energy(char* s, MATRIX coords, int n, VECTOR all_c_alpha){
	type energy=0.0;
	VECTOR c_alpha_i;
	VECTOR c_alpha_j;
	type distanza;

	#pragma omp parallel for private(c_alpha_i, c_alpha_j, distanza) reduction(+ : energy) schedule(dynamic)
	for(int i=0; i<n;i++){
		c_alpha_i =alloc_matrix(4,1);
		c_alpha_j =alloc_matrix(4,1);

		c_alpha_i[0] = all_c_alpha[i*3];
		c_alpha_i[1] = all_c_alpha[(i*3)+1];
		c_alpha_i[2] = all_c_alpha[(i*3)+2];
		c_alpha_i[3] = 0;

		int index_i = s[i] - 'A';

		for(int j=i+1; j<n; j++){
			
			c_alpha_j[0] = all_c_alpha[j*3];
			c_alpha_j[1] = all_c_alpha[(j*3)+1];
			c_alpha_j[2] = all_c_alpha[(j*3)+2];
			c_alpha_j[3] = 0;
			dist(c_alpha_i, c_alpha_j, &distanza);

			
			int index_j = s[j] - 'A';

			if(i!=j && distanza < 10.0 && charge[index_i]!=0 && charge[index_j]!=0){
				energy += (charge[index_i]*charge[index_j])/(distanza *4.0);
			}
		}
		dealloc_matrix(c_alpha_i);
		dealloc_matrix(c_alpha_j);
	}
	return energy;
}


type packing_energy(char* s, MATRIX coords, int n, VECTOR all_c_alpha){
	type energy=0.0;
	VECTOR c_alpha_i;
	VECTOR c_alpha_j;
	type distanza;	

	#pragma omp parallel for private(c_alpha_i, c_alpha_j, distanza) reduction(+ : energy) schedule(dynamic)
	for(int i=0; i<n; i++){
		c_alpha_i =alloc_matrix(4,1);
		c_alpha_j =alloc_matrix(4,1);
		
		type density=0.0;
		int index_i = s[i] - 'A';
		
		c_alpha_i[0] = all_c_alpha[i*3];
		c_alpha_i[1] = all_c_alpha[(i*3)+1];
		c_alpha_i[2] = all_c_alpha[(i*3)+2];
		c_alpha_i[3] = 0;

		for(int j=0; j<n; j++){
			int index_j = s[j] - 'A';
	
			c_alpha_j[0] = all_c_alpha[j*3];
			c_alpha_j[1] = all_c_alpha[(j*3)+1];
			c_alpha_j[2] = all_c_alpha[(j*3)+2];
			c_alpha_j[3] = 0;
			
			dist(c_alpha_i, c_alpha_j, &distanza);

			if(i!=j && distanza < 10.0){
				density += (volume[index_j]/pow(distanza,3));
			}
			
		}
		energy += pow((volume[index_i]-density),2);
		dealloc_matrix(c_alpha_i);
		dealloc_matrix(c_alpha_j);
	}
	return energy;
}

type energy(char* s, VECTOR phi, VECTOR psi, int n){
	MATRIX coords;
	type rama_e;
	type hydro_e;
	type ele_e;
	type pack_e;
	type total_energy=0.0;
	VECTOR all_c_alpha;

	coords = alloc_matrix(n*3,3);
	backbone(s, phi, psi, coords, n);
	all_c_alpha = get_C_alpha(coords, n);



	#pragma omp parallel sections
	{

		#pragma omp section
		{
			rama(phi, psi, n, &rama_e);
		}

		#pragma omp section
		{
			hydro_e = hydrophobicity_energy(s, coords, n, all_c_alpha);
		}

		#pragma omp section
		{
			ele_e = electrostatic_energy(s, coords, n, all_c_alpha);
		}

		#pragma omp section
		{
			pack_e = packing_energy(s, coords, n, all_c_alpha);
		}

	}
	


	/*
	rama(phi, psi, n, &rama_e);
	hydro_e = hydrophobicity_energy(s, coords, n ,all_c_alpha);
	ele_e = electrostatic_energy(s, coords, n, all_c_alpha);
	pack_e = packing_energy(s, coords, n, all_c_alpha);
	*/

	total_energy += rama_e*w_rama;
	total_energy += hydro_e*w_hidro;
	total_energy += ele_e*w_elec;
	total_energy += pack_e*w_pack;


	dealloc_matrix(coords);
	dealloc_matrix(all_c_alpha);
	return total_energy;
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

	type theta_phi;
	type theta_psi;
	type delta_E;
	type E_new;
	E = energy(s, phi, psi, n);
	int t=0;
	
	while (T>0.0){
		int i = (random() * n);
	
			
		theta_phi= (random()*2 * M_PI) - M_PI;
		phi[i]=phi[i]+theta_phi;

		
		theta_psi= (random()*2 * M_PI) - M_PI;
		psi[i]=psi[i]+theta_psi;

		E_new = energy(s, phi, psi, n);
		delta_E = E_new - E;

		if(delta_E <= 0){
			//Accetto la nuova configurazione
			E = E_new;
		}else{
			type P = exp(-delta_E/(k*T));
			type r = random();

			if(r <= P){
				//Accetto la nuova configurazione
				E = E_new;
			}else{
				//Rifiuto la nuova configurazione
				phi[i]=phi[i]-theta_phi;
				psi[i]=psi[i]-theta_psi;
			}
		}
		t+=1;
		T = to-sqrt(alpha*t);
	}
	input->e = E;
	input->phi = phi;
	input->psi = psi;
}


int main(int argc, char** argv) {
	char fname_phi[256];
	char fname_psi[256];
	char* seqfilename = NULL;
	clock_t t;
	double time;
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
		}else if (strcmp(argv[par],"-t") == 0) {   // AGGIUNTO PARAMETRO PER IL NUMERO DI THREAD
			par++;
			if (par >= argc) {
				printf("Missing number of threads value!\n");
				exit(1);
			}
			number_of_threads = atoi(argv[par]);
			par++;
		}else{
			printf("WARNING: unrecognized parameter '%s'!\n",argv[par]);
			par++;
		}
	}

	// Valuta quanti thread utilizzare
	//number_of_threads = omp_get_max_threads();
	// Imposto il numero di thread
	omp_set_num_threads(number_of_threads);

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
	


	time = omp_get_wtime();
	pst(input);
	time = omp_get_wtime() - time;

	
	if(!input->silent){
        printf("PST time = %.3f secs\n", time);
        printf("Energy = %f\n", input->e);
	}
	else{
		printf("%.3f\n", time);
		printf("%f\n", input->e);
	}

	//
	// Salva il risultato
	//
	sprintf(fname_phi, "out64_%d_%d_%.3f_%.3f_%.3f_phi.ds2", input->N, input->sd, input->to, input->alpha, input->k);
	save_out(fname_phi, input->phi, input->N);
	sprintf(fname_psi, "out64_%d_%d_%.3f_%.3f_%.3f_psi.ds2", input->N, input->sd, input->to, input->alpha, input->k);
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
