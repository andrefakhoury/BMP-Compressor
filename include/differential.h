#ifndef DIFFERENTIAL_H
#define DIFFERENTIAL_H

#include "utils.h"

/** Encode a sequence of values with Differential Encoding, extracting the pairs (value, size in bits) */
void differential_encode(int *vec, int len, INT_PAIR *ans);

/** Encodes a sequence of differential codes with huffman prefixes, extracting the pair (symbol, size in bits) to be written to file */
void differential_entropy_encode(INT_PAIR *vec, int len, INT_PAIR *ans);

/** Decodes a sequence of differential codes into its original values */
void differential_decode(INT_PAIR *vec, int len, int *ans);

/** Finds differential prefix */
int find_differential_prefix(int buffer, int buffer_size);

#endif
