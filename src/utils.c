#include "utils.h"
#include <assert.h>

/* Create a pair of integers */
INT_PAIR make_int_pair(int a, int b) {
	INT_PAIR ret;
	ret.first = a;
	ret.second = b;
	return ret;
}

// return number of bits of x
int number_of_bits(int x) {
	if(x == 0) return 0;
	return sizeof(x) * 8 - __builtin_clz(x);
}

// return 1-complement of x if size of it is already known
int ones_complement_with_size(int x, int sz) {
	int mask = (1 << sz) - 1;
	return (~x) & mask;
}

// return 1-complement of x (only works with positive numbers)
int ones_complement(int x) {
	return ones_complement_with_size(x, number_of_bits(x));
}

// auxiliar function that concatenates "num_bits" in a buffer "src" with value "value"
unsigned int append_bits(unsigned int src, unsigned char num_bits, unsigned int value) {
	return (src << num_bits) | value;
}

/* Clip a double to fit inside [min, max] */
unsigned int clip(const double val, const unsigned int min, const unsigned int max) {
	return val < min ? min : val > max ? max : val;
}

/** Alloc double matrix of size [width/BLOCK_SIZE*height/BLOCK_SIZE][BLOCK_SIZE * BLOCK_SIZE] */
void alloc_double_blocks(double*** blocks, const int width, const int height) {
	const int len = (width / BLOCK_SIZE) * (height / BLOCK_SIZE);
	*blocks = malloc(len * sizeof(double*));
	for (int i = 0; i < len; i++) {
		(*blocks)[i] = malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
	}
}

/** Alloc int matrix of size [width/BLOCK_SIZE*height/BLOCK_SIZE][BLOCK_SIZE * BLOCK_SIZE] */
void alloc_int_blocks(int*** blocks, const int width, const int height) {
	const int len = (width / BLOCK_SIZE) * (height / BLOCK_SIZE);
	*blocks = malloc(len * sizeof(int*));
	for (int i = 0; i < len; i++) {
		(*blocks)[i] = malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(int));
	}
}


/** Free blocks previously alloc'd */
void free_blocks(void** blocks, const int width, const int height) {
	const int len = (width / 8) * (height / 8);
	for (int i = 0; i < len; i++) {
		free(blocks[i]);
	}
	free(blocks);
}

long get_file_size(FILE * fp) {
	fseek(fp, 0L, SEEK_END);
	return ftell(fp);
}