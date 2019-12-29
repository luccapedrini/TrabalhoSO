#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>


#define TAM 20000			/*tamanho da matriz*/
#define MAX_THREADS 4	/*quantidade maxima de threads*/
int MACRO;			/*tamanho do macrobloco*/
int tamBlocos[5] = { 10000, 5000, 1000, 500, 100 };		/*tamanho de blocos testados*/

int **mat;
int qtdPrimos = 0;
int auxCol = 0;
int auxLin = 0;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_t threads[MAX_THREADS];


void criaMat() { /*funçao que cria a matriz*/
	
	int i, j;
	mat = (int**)malloc(TAM * sizeof(int*));
	int cont = 0;

	for (i = 0; i < TAM; i++) {
		mat[i] = (int*)malloc(TAM * sizeof(int));

		for (j = 0; j < TAM; j++) {
			mat[i][j] = rand() % 30000;
			cont++;
		}
	}
	return;
}

void liberaMemoria() { /*libera a memoria alocada para a matriz*/
	int cont;

	for (cont = 0; cont < TAM; cont++) {
		free(mat[cont]);
	}
	free(mat);

	return;
}

int verificaPrimo(int num) { /*verifica se o numero é primo, retorna 1 se sim*/
	int i;
	int raiz = sqrt(num);
	if (num < 2) {
		return 0;
	}
	for (i = 2; i <= raiz; i++) {
		if (num % i == 0) {
			return 0;
		}
	}
	return 1;
}

void buscaSequencial() {
	int i, j;

	for (i = 0; i < TAM; i++) {
		
		for (j = 0; j < TAM; j++) {
			
			if (verificaPrimo(mat[i][j])) {
				qtdPrimos++;
			}
		}
	}
	return;
}

void *buscaParalela(void *lixo) {
	
	int i, j;
	int cont = 0;

	int iniI = 0;
	int iniJ = 0;
	int fimI = 0;
	int fimJ = 0;

	while (auxCol < TAM && auxLin < TAM) {
		pthread_mutex_lock(&mutex1);
		iniI = auxLin;
		iniJ = auxCol;
		fimI = MACRO + iniI;
		fimJ = MACRO + iniJ;

		auxCol = auxCol + MACRO;
		if (auxCol % TAM == 0) {
			auxLin = auxLin + MACRO;
			auxCol = 0;
		}

		pthread_mutex_unlock(&mutex1);
		for (i = iniI; i < fimI; i++) {

			for (j = iniJ; j < fimJ; j++) {
				
				if (verificaPrimo(mat[i][j])) {
					cont++;
				}
			}
		}
		
		pthread_mutex_lock(&mutex2);
		qtdPrimos = qtdPrimos + cont;
		cont = 0;
		pthread_mutex_unlock(&mutex2);

	}
	pthread_exit((void *)NULL);
}

int main() {
	srand(3);
	criaMat();
	
	FILE *saida = fopen("saida2.txt", "w");


	clock_t iniTempo = clock();
	buscaSequencial();
	clock_t fimTempo = clock();
	double tempoGasto = (double)(fimTempo - iniTempo) / CLOCKS_PER_SEC;
	
	printf("Teste para busca sequencial\n");
	printf("Quantidade de primos achados : %d\n", qtdPrimos);
	printf("Tempo gasto : %lf\n\n\n", tempoGasto);

	fprintf(saida, "Teste para busca sequencial\n");
	fprintf(saida, "Quantidade de primos achados : %d\n", qtdPrimos);
	fprintf(saida, "Tempo gasto : %lf\n\n\n", tempoGasto);
	qtdPrimos = 0;
	
	///-----------------------------------------------------------
	
	int i, j, k;
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	
	for (j = 2; j <= MAX_THREADS; j++) {

		for (k = 0; k < 5; k++) {
			iniTempo = clock();
			MACRO = tamBlocos[k];
			
			for (i = 0; i < j; i++) {
				pthread_create(&(threads[i]), NULL, buscaParalela, NULL);
				
			}

			for (i = 0; i < j; i++) {
				pthread_join(threads[i], NULL);
			}
			fimTempo = clock();
			tempoGasto = (double)(fimTempo - iniTempo) / CLOCKS_PER_SEC;
			printf("Teste para busca paralela com %d threads e macrobloco de %d elementos\n",j,MACRO);
			printf("Quantidade de primos achados : %d\n", qtdPrimos);
			printf("Tempo gasto : %lf\n\n\n", tempoGasto);

			fprintf(saida, "Teste para busca paralela com %d threads e macrobloco de %d elementos\n", j, MACRO);
			fprintf(saida, "Quantidade de primos achados : %d\n", qtdPrimos);
			fprintf(saida, "Tempo gasto : %lf\n\n\n", tempoGasto);
		
			qtdPrimos = 0;
			auxCol = 0;
			auxLin = 0;
		}
		
		
	}

	liberaMemoria();
	
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	fclose(saida);
	system("pause");
	
	return 0;
}