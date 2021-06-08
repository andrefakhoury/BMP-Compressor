#ifndef UTILS_H
#define UTILS_H

#include "bmp.h"
#include <stdlib.h>

/** Clips value 'val' to unsigned int of range[min, max] */
unsigned int clip(const double val, const unsigned int min, const unsigned int max);

/** Alloc double matrix of of size [width/8*height/8][64] */
void alloc_blocks(double*** blocks, const int width, const int height);

/** Free blocks previously alloc'd */
void free_blocks(double** blocks, const int width, const int height);

#endif