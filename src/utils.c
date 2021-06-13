#include "utils.h"

unsigned int clip(const double val, const unsigned int min, const unsigned int max) {
	return val < min ? min : val > max ? max : val;
}

/** Alloc double matrix of of size [width/8*height/8][64] */
void alloc_blocks(double*** blocks, const int width, const int height) {
	const int len = (width / 8) * (height / 8);
	*blocks = malloc(len * sizeof(double*));
	for (int i = 0; i < len; i++) {
		(*blocks)[i] = malloc(64 * sizeof(double));
	}
}

/** Free blocks previously alloc'd */
void free_blocks(double** blocks, const int width, const int height) {
	const int len = (width / 8) * (height / 8);
	for (int i = 0; i < len; i++) {
		free(blocks[i]);
	}
	free(blocks);
}
