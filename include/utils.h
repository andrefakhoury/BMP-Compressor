#ifndef UTILS_H
#define UTILS_H

#include "bmp.h"
#include <stdlib.h>

#define BLOCK_SIZE 8

// Pair of integers utils
typedef struct {
	int first, second;
} INT_PAIR;

/* Creates an integer pair */
INT_PAIR make_int_pair(int a, int b);

// return 1-complement of x (only works with positive numbers)
int ones_complement(int x);

// return 1-complement of x if size of it is already known
int ones_complement_with_size(int x, int sz);

// return number of bits of x
int number_of_bits(int x);

// auxiliar function that concatenates "num_bits" in a buffer "src" with value "value"
unsigned int append_bits(unsigned int src, unsigned char num_bits, unsigned int value);

/** Clips value 'val' to unsigned int of range[min, max] */
unsigned int clip(const double val, const unsigned int min, const unsigned int max);

/** Alloc double matrix of size [width/BLOCK_SIZE*height/BLOCK_SIZE][BLOCK_SIZE * BLOCK_SIZE] */
void alloc_double_blocks(double*** blocks, const int width, const int height);

/** Alloc double matrix of size [width/BLOCK_SIZE*height/BLOCK_SIZE][BLOCK_SIZE * BLOCK_SIZE] */
void alloc_int_blocks(int*** blocks, const int width, const int height);

/** Free blocks previously alloc'd */
void free_blocks(void** blocks, const int width, const int height);

/* Returns a file's size in bytes */
long get_file_size(FILE * fp);

#endif