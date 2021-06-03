#include "dct.h"
#include <math.h>

// Aplica algoritmo DTC em bloco 8x8
void DCT(float* block, float* result) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			// calculates result[i][j]
			float sum = 0.0;
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					
					sum += block[x * 8 + y] * cos(((2 * x + 1) * i * M_PI)/16) * cos(((2 * y + 1) * j * M_PI)/16);
				}
			}

			const float c_i = i ? 1 : 1 / sqrt(2);
			const float c_j = j ? 1 : 1 / sqrt(2);
			
			result[i * 8 + j] = c_i * c_j * sum / 4;
		}
	}
}

// Aplica algoritmo de DCT inverso em bloco 8x8
void IDCT(float* block, float* result) {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			
			float sum = 0.0;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					const float c_i = i ? 1 : 1 / sqrt(2);
					const float c_j = j ? 1 : 1 / sqrt(2);	
					
					sum += block[i * 8 + j] * c_i * c_j * cos(((2 * x + 1) * i * M_PI)/16) * cos(((2 * y + 1) * j * M_PI)/16);
				}
			}

			result[x * 8 + y] = sum / 4;
		}
	}
}
