#ifndef ENCODING_H
#define ENCODING_H

#include "utils.h"

/** Apply differential and RLE encoding into DCs and ACs, and returns a list of codes to write to file */
void encode_blocks(int **blocks, int num_blocks, INT_PAIR *ans, int *ans_len);

/** Decodes fp and loads the result into blocks */
void decode_blocks(FILE *fp, int **blocks, int num_blocks);

#endif
