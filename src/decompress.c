#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "utils.h"
#include "bmp.h"
#include "preparation.h"
#include "dct.h"
#include "encoding.h"
#include "rle.h"
#include "bits.h"

#define BLOCK_SIZE 8

void pre() {
	read_rle_prefixes();
}

void decompress_channel(FILE * fp, double * I, int original_width, int original_height, int is_subsampled) {
	int width, height;

	if(is_subsampled) {
		width = (original_width + 1) / 2;
		height = (original_height + 1) / 2;
	} else {
		width = original_width;
		height = original_height;
	}

	// padding to divide into even blocks
	const int pad_width = width + (8 - width % 8) % 8;
	const int pad_height = height + (8 - height % 8) % 8;

	const int num_blocks = (pad_width / BLOCK_SIZE) * (pad_height / BLOCK_SIZE);

	// decode blocks
	int ** int_blocks;
	alloc_int_blocks(&int_blocks, pad_width, pad_height);

	decode_blocks(fp, int_blocks, num_blocks);

	// inverse of quantization
	double ** blocks;
	alloc_double_blocks(&blocks, pad_width, pad_height);

	reverse_quantization(int_blocks, blocks, quant_table, num_blocks);

	// inverse DCT process
	idct(blocks, num_blocks);

	// merge blocks
	double * I_pad = malloc(pad_width * pad_height * sizeof(double));

	merge_blocks(blocks, I_pad, pad_width, pad_height);

	// reverse padding
	double * I_sub = malloc(width * height * sizeof(double));
	padding(I_pad, I_sub, pad_width, width, pad_height, height);

	// reverse subsample
	if(is_subsampled) {
		reverse_subsample(I_sub, I, original_width, original_height);
	} else {
		// copy I_sub to I
		memcpy(I, I_sub, sizeof(double) * width * height);
	}

	free_blocks((void **) int_blocks, pad_width, pad_height);
	free_blocks((void **) blocks, pad_width, pad_height);

	free(I_sub);
	free(I_pad);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
		printf("Usage: %s <imagem_entrada>\n", argv[0]);
		return 0;
	}

	pre(); // do some preparation work
	
	// Open input image
	FILE* input = fopen(argv[1], "rb");
	if (input == NULL) {
		fprintf(stderr, "Error: could not open input file\n");
		return 1;
	}

	BMPFILEHEADER fileheader; /* File header */
	BMPINFOHEADER infoheader; /* Info header */

	// load headers to structs
	bmp_load_headers(input, &fileheader, &infoheader);

	const int width = infoheader.width;
	const int height = infoheader.height;
	int img_size = width * height;

	// alloc image and load rgb pixels
	RGBPIXEL* img = malloc(img_size * sizeof(RGBPIXEL));

	// Decompress channels
	double *Y = malloc(img_size * sizeof(double));
	double *Cb = malloc(img_size * sizeof(double));
	double *Cr = malloc(img_size * sizeof(double));

	decompress_channel(input, Y, width, height, 0);
	decompress_channel(input, Cb, width, height, 1);
	decompress_channel(input, Cr, width, height, 1);

	ycbcr_to_rgb(Y, Cb, Cr, img, img_size);

	// save output image
	FILE *output = fopen("img_save.bmp", "wb");
	bmp_copy_headers(input, output);
	bmp_write_image(output, img, img_size);

	fclose(input);
	fclose(output);

	free(img);
	free(Y);
	free(Cb);
	free(Cr);

	return 0;
}
