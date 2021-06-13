#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "utils.h"
#include "bmp.h"
#include "preparation.h"
#include "dct.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
		printf("Usage: %s <imagem_entrada>\n", argv[0]);
		return 0;
	}
	
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

	// subsampling
	const int sub_width = (width + 1) / 2;
	const int sub_height = (height + 1) / 2;

	double *Cb_sub = malloc(sub_width * sub_height * sizeof(double));
	double *Cr_sub = malloc(sub_width * sub_height * sizeof(double));

	subsample(Cb, Cb_sub, width, height);
	subsample(Cr, Cr_sub, width, height);

	// padding to divide into even blocks
	const int pad_width = width + (8 - width % 8) % 8;
	const int pad_sub_width = sub_width + (8 - sub_width % 8) % 8;
	const int pad_height = height + (8 - height % 8) % 8;
	const int pad_sub_height = sub_height + (8 - sub_height % 8) % 8;

	double *Y_pad = malloc(pad_width * pad_height * sizeof(double));
	double *Cb_pad = malloc(pad_sub_width * pad_sub_height * sizeof(double));
	double *Cr_pad = malloc(pad_sub_width * pad_sub_height * sizeof(double));

	padding(Y, Y_pad, width, pad_width, height, pad_height);
	padding(Cb_sub, Cb_pad, sub_width, pad_sub_width, sub_height, pad_sub_height);
	padding(Cr_sub, Cr_pad, sub_width, pad_sub_width, sub_height, pad_sub_height);

	// divide into blocks
	double **Y_block, **Cb_block, **Cr_block;
	alloc_blocks(&Y_block, pad_width, pad_height);
	alloc_blocks(&Cb_block, pad_sub_width, pad_sub_height);
	alloc_blocks(&Cr_block, pad_sub_width, pad_sub_height);

	divide_in_blocks(Y_pad, Y_block, pad_width, pad_height);
	divide_in_blocks(Cb_pad, Cb_block, pad_sub_width, pad_sub_height);
	divide_in_blocks(Cr_pad, Cr_block, pad_sub_width, pad_sub_height);

	// apply DCT on each block of each channel
	dct(Y_block, (pad_width / 8) * (pad_height / 8));
	dct(Cb_block, (pad_sub_width / 8) * (pad_sub_height / 8));
	dct(Cr_block, (pad_sub_width / 8) * (pad_sub_height / 8));

	// apply quantization
	quantization(Y_block, quant_table, (pad_width / 8) * (pad_height / 8));
	quantization(Cb_block, quant_table, (pad_sub_width / 8) * (pad_sub_height / 8));
	quantization(Cr_block, quant_table, (pad_sub_width / 8) * (pad_sub_height / 8));

	/* =-=-=-=-=-=-=-=-=-=-=-= Uncompressing process =-=-=-=-=-=-=-=-=-=-=-= */
	memset(Y_pad, 0, pad_width * pad_height * sizeof(double));
	memset(Cb_pad, 0, pad_sub_width * pad_sub_height * sizeof(double));
	memset(Cr_pad, 0, pad_sub_width * pad_sub_height * sizeof(double));
	memset(Cb_sub, 0, sub_width * sub_height * sizeof(double));
	memset(Cr_sub, 0, sub_width * sub_height * sizeof(double));
	memset(Y, 0, width * height * sizeof(double));
	memset(Cb, 0, width * height * sizeof(double));
	memset(Cr, 0, width * height * sizeof(double));

	// inverse of quantization
	reverse_quantization(Y_block, quant_table, (pad_width / 8) * (pad_height / 8));
	reverse_quantization(Cb_block, quant_table, (pad_sub_width / 8) * (pad_sub_height / 8));
	reverse_quantization(Cr_block, quant_table, (pad_sub_width / 8) * (pad_sub_height / 8));

	// inverse DCT process
	idct(Y_block, (pad_width / 8) * (pad_height / 8));
	idct(Cb_block, (pad_sub_width / 8) * (pad_sub_height / 8));
	idct(Cr_block, (pad_sub_width / 8) * (pad_sub_height / 8));

	// merge blocks
	merge_blocks(Y_block, Y_pad, pad_width, pad_height);
	merge_blocks(Cb_block, Cb_pad, pad_sub_width, pad_sub_height);
	merge_blocks(Cr_block, Cr_pad, pad_sub_width, pad_sub_height);

	// reverse padding
	padding(Y_pad, Y, pad_width, width, pad_height, height);
	padding(Cb_pad, Cb_sub, pad_sub_width, sub_width, pad_sub_height, sub_height);
	padding(Cr_pad, Cr_sub, pad_sub_width, sub_width, pad_sub_height, sub_height);

	// reverse subsample
	reverse_subsample(Cb_sub, Cb, width, height);
	reverse_subsample(Cr_sub, Cr, width, height);

	// convert back to RGB
	ycbcr_to_rgb(Y, Cb, Cr, img, img_size);

	// save output image
	FILE *output = fopen("img_save.bmp", "wb");
	bmp_copy_headers(input, output);
	bmp_write_image(output, img, img_size);

	// close files and free heap memory
	free_blocks(Y_block, pad_width, pad_height);
	free_blocks(Cb_block, pad_sub_width, pad_sub_height);
	free_blocks(Cr_block, pad_sub_width, pad_sub_height);

	free(Cb_sub);
	free(Cr_sub);

	free(Y_pad);
	free(Cb_pad);
	free(Cr_pad);

	fclose(output);
	fclose(input);
	free(img);
	free(Y);
	free(Cb);
	free(Cr);
}
