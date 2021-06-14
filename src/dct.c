#include <math.h>

#include "dct.h"

/** Apply DCT to each 64-element block in blocks */
void dct(double** blocks, const int qt_blocks) {
	double _cos[8][8];
	const double sq2_inv = 1.0 / sqrt(2.0);

	// preprocess cosine of (2*x+1)*i*pi/16 to reduce run time
	for (int x = 0; x < 8; x++) {
		for (int i = 0; i < 8; i++) {
			_cos[x][i] = cos((2 * x + 1) * i * M_PI / 16);
		}
	}

	// auxiliar matrix with previous values
	double aux[8][8];

	// apply dct for each block
	for (int bl = 0; bl < qt_blocks; bl++) {
		// fill previous values
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				aux[i][j] = blocks[bl][i * 8 + j];

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				double sum = 0;
				for (int x = 0; x < 8; x++) {
					for (int y = 0; y < 8; y++) {						
						sum += aux[x][y] * _cos[x][i] * _cos[y][j];
					}
				}
				const double c_i = i ? 1 : sq2_inv;
				const double c_j = j ? 1 : sq2_inv;

				blocks[bl][i * 8 + j] = c_i * c_j * sum / 4;
			}
		}
	}
}

/** Apply Inverse DCT to each block */
void idct(double** blocks, const int qt_blocks) {
	double _cos[8][8];
	double sq2_inv = 1.0 / sqrt(2.0);

	// preprocess cosine of (2*x+1)*i*pi/16 to reduce run time
	for (int x = 0; x < 8; x++) {
		for (int i = 0; i < 8; i++) {
			_cos[x][i] = cos((2 * x + 1) * i * M_PI / 16);
		}
	}

	// auxiliar matrix with previous values
	double aux[8][8];

	for (int bl = 0; bl < qt_blocks; bl++) {
		// fill previous values
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				aux[i][j] = blocks[bl][i * 8 + j];

		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				double sum = 0.0;
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 8; j++) {
						const double c_i = i ? 1 : sq2_inv;
						const double c_j = j ? 1 : sq2_inv;	
						sum += aux[i][j] * c_i * c_j * _cos[x][i] * _cos[y][j];
					}
				}
				blocks[bl][x * 8 + y] = sum / 4;
			}
		}
	}
}

/** Apply quantization to blocks (of size 64), according to given quantization table */
void quantization(double** blocks, int** result, const int* table, const int qt_blocks) {
	const double k = 1; // compress factor
	for (int bl = 0; bl < qt_blocks; bl++) {
		for (int i = 0; i < 64; i++) {
			result[bl][i] = (int) round(blocks[bl][i] / table[i] / k);
		}		
	}
}

/** Reverse the quantization process, according to given quantization table */
void reverse_quantization(int** blocks, double** result, const int* table, const int qt_blocks) {
	const double k = 1; // compress factor
	for (int bl = 0; bl < qt_blocks; bl++) {
		for (int i = 0; i < 64; i++) {
			result[bl][i] = (double) blocks[bl][i] * table[i] * k;
		}
	}
}
