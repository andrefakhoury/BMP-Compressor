#ifndef COMPRESS_H
#define COMPRESS_H

#include "bmp.h"

/* Huffman prefixes */
#define QT_PREF 9
const static unsigned char prefTable[QT_PREF] = {2, 3, 4, 0, 5, 6, 14, 30, 62};
const static unsigned char prefSize[QT_PREF] = {3, 3, 3, 2, 3, 3, 4, 5, 6};

typedef struct {
	unsigned int code;
	unsigned char size;
} VALUE;

// apply differential encoding to G channel of bmp image
void differential_compression(VALUE* values, BMPPIXEL* img, int height, int width);

// apply huffman and save bits to fp
void save_bits(FILE* fp, VALUE* values, int size);

// 
void load_bits(FILE* fp, VALUE* values);

#endif
