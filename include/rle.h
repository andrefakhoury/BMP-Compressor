#ifndef RLE_H
#define RLE_H

#include "utils.h"

/** Encode a sequence of values with Run Length Encoding, extracting the pairs (number of zeros, value) */
void rle_encode(int *vec, int len, INT_PAIR *ans, int *ans_len);

/** Decodes a sequence of RLE codes into its original values */
void rle_decode(INT_PAIR *vec, int len, int *ans, int ans_len);

/** Encodes RLE codes using Huffman prefixes */
void rle_entropy_encode(INT_PAIR *vec, int len, INT_PAIR *ans);

/** Finds a RLE prefix */
INT_PAIR find_rle_prefix(int x, int len);

/** Read huffman prefixes from file */
void read_rle_prefixes();

#endif
