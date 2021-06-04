#include <stdio.h>
#include <stdlib.h>

const int BLOCK_SIZE = 8;

// faz o zigzag em vec, cujo tamanho deve ser BLOCK_SIZE^2, e coloca o resultado no proprio vec
void zigzag(int* vec) {
	int * ans = (int *) malloc(sizeof(int) * BLOCK_SIZE * BLOCK_SIZE); // aloca um vetor auxiliar para armazenar os elementos em ordem zig zag
	for(int i = 0, j = 0, pos = 0; i + j <= BLOCK_SIZE + BLOCK_SIZE; pos++) {
		// faz a transicao para a proxima diagonal
		if(i == BLOCK_SIZE) i = BLOCK_SIZE-1, j += 2;
		if(j == BLOCK_SIZE) j = BLOCK_SIZE-1, i += 2;
		if(i < 0) i++;
		if(j < 0) j++;


		ans[pos] = vec[i * BLOCK_SIZE + j]; // copia de vec para ans
		
		if((i + j)%2) { // desce
			i++;
			j--;
		} else { // sobe
			i--;
			j++;
		}
	}

	// copia ans para vec
	for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++) {
		vec[i] = ans[i];
	}

	free(ans);
}

// faz o inverso do zigzag em vec, cujo tamanho deve ser BLOCK_SIZE^2, e coloca o resultado no proprio vec
void zagzig(int * vec) {
	int * ans = (int *) malloc(sizeof(int) * BLOCK_SIZE * BLOCK_SIZE); // aloca um vetor auxiliar para armazenar os elementos na ordem original

	for(int i = 0, j = 0, pos = 0; i + j <= BLOCK_SIZE + BLOCK_SIZE; pos++) {
		// faz a transicao para a proxima diagonal
		if(i == BLOCK_SIZE) i = BLOCK_SIZE-1, j += 2;
		if(j == BLOCK_SIZE) j = BLOCK_SIZE-1, i += 2;
		if(i < 0) i++;
		if(j < 0) j++;

		ans[i * BLOCK_SIZE + j] = vec[pos]; // copia de vec para ans

		if((i + j)%2) { // desce
			i++;
			j--;
		} else { // sobe
			i--;
			j++;
		}
	}
	// copia ans para vec
	for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++) {
		vec[i] = ans[i];
	}

	free(ans);
}

int test_zigzag() {
	int * vec = (int *) malloc(sizeof(int) * BLOCK_SIZE * BLOCK_SIZE);

	for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; ++i) {
		vec[i] = i+1;
	}

	for(int i = 0; i < BLOCK_SIZE; ++i) {
		for(int j = 0; j < BLOCK_SIZE; ++j) {
			printf("%2d ", vec[i * BLOCK_SIZE + j]);		
		}
		printf("\n");
	}

	zigzag(vec);

	for(int i = 0; i < BLOCK_SIZE; ++i) {
		for(int j = 0; j < BLOCK_SIZE; ++j) {
			printf("%2d ", vec[i * BLOCK_SIZE + j]);		
		}
		printf("\n");
	}

	zagzig(vec);

	for(int i = 0; i < BLOCK_SIZE; ++i) {
		for(int j = 0; j < BLOCK_SIZE; ++j) {
			printf("%2d ", vec[i * BLOCK_SIZE + j]);		
		}
		printf("\n");
	}

	free(vec);
}

/*
	ACs must have size num_blocks * (BLOCK_SIZE * BLOCK_SIZE - 1)
	DCs must have size num_blocks
*/
void extract_acdc(int ** blocks, int num_blocks, int * ACs, int * DCs) {
	
	for(int i = 0, j = 0; i < num_blocks; i++) {
		DCs[i] = blocks[i][0];
		for(int k = 1; k < BLOCK_SIZE * BLOCK_SIZE; k++)
			ACs[j++] = blocks[i][k];
	}
}

int main() {


}