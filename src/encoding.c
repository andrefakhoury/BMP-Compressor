#include <stdio.h>
#include <stdlib.h>
#include "differential.h"
#include "rle.h"
#include "encoding.h"
#include "bits.h"

// faz o zigzag em vec, cujo tamanho deve ser BLOCK_SIZE^2, e coloca o resultado no proprio vec
void zigzag(int* vec) {
	int * ans = (int *) malloc(sizeof(int) * BLOCK_SIZE * BLOCK_SIZE); // aloca um vetor auxiliar para armazenar os elementos em ordem zig zag
	for(int i = 0, j = 0, pos = 0; pos < BLOCK_SIZE * BLOCK_SIZE; pos++) {
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

	for(int i = 0, j = 0, pos = 0; pos < BLOCK_SIZE * BLOCK_SIZE; pos++) {
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

void merge_acdc(int ** blocks, int num_blocks, int * ACs, int * DCs) {	
	for(int i = 0, j = 0; i < num_blocks; i++) {
		blocks[i][0] = DCs[i];
		for(int k = 1; k < BLOCK_SIZE * BLOCK_SIZE; k++)
			blocks[i][k] = ACs[j++];
	}
}

// UNTESTED
void encode_blocks(int ** blocks, int num_blocks, INT_PAIR * ans, int * ans_len) {

	int len_dcs = num_blocks;
	int len_acs = num_blocks * (BLOCK_SIZE * BLOCK_SIZE - 1);

	int * ACs = malloc(sizeof(int) * len_acs);
	int * DCs = malloc(sizeof(int) * len_dcs);
	for(int i = 0; i < num_blocks; i++) {
		zigzag(blocks[i]);
	}

	extract_acdc(blocks, num_blocks, ACs, DCs);

	INT_PAIR * DC_codes = malloc(sizeof(INT_PAIR) * len_dcs);
	INT_PAIR * AC_codes = malloc(sizeof(INT_PAIR) * len_acs);

	(*ans_len) = 0;
	
	differential_encode(DCs, len_dcs, DC_codes);
	differential_entropy_encode(DC_codes, len_dcs, ans);

	(*ans_len) += len_dcs;

	int rle_size_used;
	rle_encode(ACs, len_acs, AC_codes, &rle_size_used);
	rle_entropy_encode(AC_codes, rle_size_used, ans + (*ans_len));

	(*ans_len) += rle_size_used;

	free(ACs);
	free(DCs);
	free(DC_codes);
	free(AC_codes);
}

void decode_blocks(FILE * fp, int ** blocks, int num_blocks) {
	int len_dcs = num_blocks;
	int len_acs = num_blocks * (BLOCK_SIZE * BLOCK_SIZE - 1);

	INT_PAIR * DC_codes = malloc(sizeof(INT_PAIR) * len_dcs);
	INT_PAIR * AC_codes = malloc(sizeof(INT_PAIR) * len_acs);
	
	int len_ac_codes;
	load_bits(fp, DC_codes, AC_codes, len_dcs, &len_ac_codes);

	int * DCs = malloc(sizeof(int) * len_dcs);
	int * ACs = malloc(sizeof(int) * len_acs);

	differential_decode(DC_codes, len_dcs, DCs);
	rle_decode(AC_codes, len_ac_codes, ACs, len_acs);

	merge_acdc(blocks, num_blocks, ACs, DCs);

	for(int i = 0; i < num_blocks; ++i) {
		zagzig(blocks[i]);
	}

	free(ACs);
	free(DCs);
	free(DC_codes);
	free(AC_codes);
}