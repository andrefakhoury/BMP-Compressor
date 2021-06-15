#include <stdio.h>
#include <stdlib.h>
#include "differential.h"
#include "rle.h"
#include "encoding.h"
#include "bits.h"

/** Rearranges vec with zigzag order */
void zigzag(int* vec) {
	// allocates auxiliar vector to store the values in new order
	int * ans = (int *) malloc(sizeof(int) * BLOCK_SIZE * BLOCK_SIZE); 
	for(int i = 0, j = 0, pos = 0; pos < BLOCK_SIZE * BLOCK_SIZE; pos++) {
		// makes transition to next diagonal
		if(i == BLOCK_SIZE) i = BLOCK_SIZE-1, j += 2;
		if(j == BLOCK_SIZE) j = BLOCK_SIZE-1, i += 2;
		if(i < 0) i++;
		if(j < 0) j++;

		ans[pos] = vec[i * BLOCK_SIZE + j]; // copy to auxiliary vector
		
		if((i + j)%2) { // go down
			i++;
			j--;
		} else { // go up
			i--;
			j++;
		}
	}

	// copy from auxiliary vector to original one
	for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++) {
		vec[i] = ans[i];
	}

	free(ans);
}

/** Do the opposite of zigzag, rearranging from zigzag order to original order */
void zagzig(int * vec) {
	// allocates auxiliar vector to store the values in new order
	int * ans = (int *) malloc(sizeof(int) * BLOCK_SIZE * BLOCK_SIZE);

	for(int i = 0, j = 0, pos = 0; pos < BLOCK_SIZE * BLOCK_SIZE; pos++) {
		// makes transition to next diagonal
		if(i == BLOCK_SIZE) i = BLOCK_SIZE-1, j += 2;
		if(j == BLOCK_SIZE) j = BLOCK_SIZE-1, i += 2;
		if(i < 0) i++;
		if(j < 0) j++;

		ans[i * BLOCK_SIZE + j] = vec[pos]; // copy to auxiliary vector

		if((i + j)%2) { // go down
			i++;
			j--;
		} else { // go up
			i--;
			j++;
		}
	}

	// copy from auxiliary vector to original one
	for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++) {
		vec[i] = ans[i];
	}

	free(ans);
}


/** Extract vectors of AC values and DC values from blocks */
void extract_acdc(int ** blocks, int num_blocks, int * ACs, int * DCs) {
	
	for(int i = 0, j = 0; i < num_blocks; i++) {
		DCs[i] = blocks[i][0];
		for(int k = 1; k < BLOCK_SIZE * BLOCK_SIZE; k++)
			ACs[j++] = blocks[i][k];
	}
}

/** Merge vectors of AC values and DC values into blocks */
void merge_acdc(int ** blocks, int num_blocks, int * ACs, int * DCs) {	
	for(int i = 0, j = 0; i < num_blocks; i++) {
		blocks[i][0] = DCs[i];
		for(int k = 1; k < BLOCK_SIZE * BLOCK_SIZE; k++)
			blocks[i][k] = ACs[j++];
	}
}


void encode_blocks(int ** blocks, int num_blocks, INT_PAIR * ans, int * ans_len) {
	// Allocate vectors of AC and DC values
	int len_dcs = num_blocks;
	int len_acs = num_blocks * (BLOCK_SIZE * BLOCK_SIZE - 1);

	int * ACs = malloc(sizeof(int) * len_acs);
	int * DCs = malloc(sizeof(int) * len_dcs);

	// Rearrange into zigzag order
	for(int i = 0; i < num_blocks; i++) {
		zigzag(blocks[i]);
	}

	// Extract DCs and ACs
	extract_acdc(blocks, num_blocks, ACs, DCs);

	// Allocate space for AC and DC codes
	INT_PAIR * DC_codes = malloc(sizeof(INT_PAIR) * len_dcs);
	INT_PAIR * AC_codes = malloc(sizeof(INT_PAIR) * len_acs);

	(*ans_len) = 0;
	
	// Do differential encoding
	differential_encode(DCs, len_dcs, DC_codes);
	differential_entropy_encode(DC_codes, len_dcs, ans);

	(*ans_len) += len_dcs;

	// Do RLE encoding
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
	// Allocate vectors of AC and DC codes
	int len_dcs = num_blocks;
	int len_acs = num_blocks * (BLOCK_SIZE * BLOCK_SIZE - 1);

	INT_PAIR * DC_codes = malloc(sizeof(INT_PAIR) * len_dcs);
	INT_PAIR * AC_codes = malloc(sizeof(INT_PAIR) * len_acs);
	
	// Load bits from file and extract AC and DC codes
	int len_ac_codes;
	load_bits(fp, DC_codes, AC_codes, len_dcs, &len_ac_codes);

	// Allocate vectors of AC and DC values
	int * DCs = malloc(sizeof(int) * len_dcs);
	int * ACs = malloc(sizeof(int) * len_acs);

	// Do differential and RLE decoding
	differential_decode(DC_codes, len_dcs, DCs);
	rle_decode(AC_codes, len_ac_codes, ACs, len_acs);

	// Merge ACs and DCs int blocks
	merge_acdc(blocks, num_blocks, ACs, DCs);

	// Rearrange from zigzag order
	for(int i = 0; i < num_blocks; ++i) {
		zagzig(blocks[i]);
	}

	free(ACs);
	free(DCs);
	free(DC_codes);
	free(AC_codes);
}
