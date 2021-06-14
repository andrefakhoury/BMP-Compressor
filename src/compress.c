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

void compress_channel(FILE * fp, double * I, int width, int height, int should_subsample) {
	double * I_sub = NULL;

	if(should_subsample) { // subsampling
		const int sub_width = (width + 1) / 2;
		const int sub_height = (height + 1) / 2;

		I_sub = malloc(sub_width * sub_height * sizeof(double));

		subsample(I, I_sub, width, height);
		
		I = I_sub;
		width = sub_width;
		height = sub_height;
	}

	// padding to divide into even blocks
	const int pad_width = width + (8 - width % 8) % 8;
	const int pad_height = height + (8 - height % 8) % 8;

	double * I_pad = malloc(pad_width * pad_height * sizeof(double));

	padding(I, I_pad, width, pad_width, height, pad_height);

	// divide into blocks
	double ** I_blocks;
	alloc_double_blocks(&I_blocks, pad_width, pad_height);

	divide_in_blocks(I_pad, I_blocks, pad_width, pad_height);

	assert(pad_width % BLOCK_SIZE == 0);
	assert(pad_height % BLOCK_SIZE == 0);
	const int num_blocks = (pad_width / BLOCK_SIZE) * (pad_height / BLOCK_SIZE);
	// apply DCT on each block
	dct(I_blocks, num_blocks);

	// allocate int blocks
	int ** I_result_blocks;
	alloc_int_blocks(&I_result_blocks, pad_width, pad_height);

	// apply quantization
	quantization(I_blocks, I_result_blocks, quant_table, num_blocks);

	// encode blocks
	INT_PAIR * codes = (INT_PAIR *) malloc(num_blocks * BLOCK_SIZE * BLOCK_SIZE * sizeof(INT_PAIR));

	int len; // number of codes
	encode_blocks(I_result_blocks, num_blocks, codes, &len);

	// write to file
	write_bits(codes, len, fp);

	free(I_pad);
	free_blocks((void **) I_blocks, pad_width, pad_height);
	free_blocks((void **) I_result_blocks, pad_width, pad_height);
	if(I_sub) free(I_sub);

	free(codes);
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

	bmp_load_image(input, img, img_size);

	/* =-=-=-=-=-=-=-=-=-=-=-= Compressing process =-=-=-=-=-=-=-=-=-=-=-= */

	// Convert RGB to YCbCr
	double *Y = malloc(img_size * sizeof(double));
	double *Cb = malloc(img_size * sizeof(double));
	double *Cr = malloc(img_size * sizeof(double));
	rgb_to_ycbcr(img, Y, Cb, Cr, img_size);

	FILE* output = fopen("out.bin", "wb");

	bmp_copy_headers(input, output);

	compress_channel(output, Y, width, height, 0);
	compress_channel(output, Cb, width, height, 1);
	compress_channel(output, Cr, width, height, 1);

	// Show compression rate
	double original_size = (double) get_file_size(input);
	double compressed_size = (double) get_file_size(output);

	printf("Taxa de compressão: %.2lf\n", original_size/compressed_size);

	free(img);
	free(Y);
	free(Cb);
	free(Cr);

	fclose(input);
	fclose(output);
}
