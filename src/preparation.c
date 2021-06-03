#include "preparation.h"

void rgb_to_yuv(RGBPIXEL* image, YUVPIXEL* result, int length) {
	for (int i = 0; i < length; i++) {
		result[i].Y = 0.299 * image[i].R + 0.587 * image[i].G + 0.114 * image[i].B;
		result[i].U = (image[i].B - result[i].Y) / 2 + 0.5;
		result[i].V = (image[i].R - result[i].Y) / 1.6 + 0.5;
	}
}

void extract_channels(YUVPIXEL* image, float* Y, float* U, float* V, int length) {
	for (int i = 0; i < length; i++) {
		Y[i] = image[i].Y;
		U[i] = image[i].U;
		V[i] = image[i].V;
	}
}

void divide_in_blocks(float* image, float** result, int width, int height) {
	// result ja ta alocado com w/8 * h/8 posices, e result[i] ja eh um vetor de 64 posicoes

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int pos = i/8 * (width/8) + j/8;
			result[pos][(i % 8) * 8 + j % 8] = image[i * width + j];
		}
	}
}
