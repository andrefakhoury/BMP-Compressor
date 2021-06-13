#ifndef DCT_H
#define DCT_H

// Default quantization table to be used.
const static int quant_table[] = {17, 18, 24, 47, 99, 99, 99, 99,
                                  18, 21, 26, 66, 99, 99, 99, 99,
                                  24, 26, 56, 99, 99, 99, 99, 99,
                                  47, 66, 99, 99, 99, 99, 99, 99,
                                  99, 99, 99, 99, 99, 99, 99, 99,
                                  99, 99, 99, 99, 99, 99, 99, 99,
                                  99, 99, 99, 99, 99, 99, 99, 99,
                                  99, 99, 99, 99, 99, 99, 99, 99};

/** Apply DCT to each 64-element block in blocks */
void dct(double** blocks, const int qt_blocks);

/** Apply Inverse DCT to each block */
void idct(double** blocks, const int qt_blocks);

/** Apply quantization to blocks (of size 64), according to given quantization table */
void quantization(double** blocks, const int* table, const int qt_blocks);

/** Reverse the quantization process, according to given quantization table */
void reverse_quantization(double** blocks, const int* table, const int qt_blocks);

#endif
