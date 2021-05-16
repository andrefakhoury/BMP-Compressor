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

// applies differential encoding to G channel of bmp image
VALUE* differential_compression(BMPPIXEL* img, int height, int width);

// applies huffman and save bits to fp
void write_bits(VALUE* values, int size, FILE * fp);

// loads bits from fp into array of values 
void load_bits(FILE* fp, VALUE* values);

#endif