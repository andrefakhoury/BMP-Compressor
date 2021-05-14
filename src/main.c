#include "bmp.h"
#include "compress.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	// read input image
	FILE* input = fopen("img/lenna.bmp", "rb");
	if (input == NULL) {
		fprintf(stderr, "Error: could not open input file\n");
		return 1;
	}

	BMPFILEHEADER fileheader;
	BMPINFOHEADER infoheader;
	bmp_load_headers(input, &fileheader, &infoheader);
	bmp_print_headers(stdout, &fileheader, &infoheader);

	int img_size = infoheader.width * infoheader.height;
	BMPPIXEL* img = malloc(img_size * sizeof(BMPPIXEL));

	bmp_load_image(input, img, img_size);

	// generate grayscale version of img
	FILE* gray = fopen("img/gray.bmp", "wb");
	if (gray == NULL) {
		fprintf(stderr, "Error: could not open gray file\n");
		return 1;
	}

	bmp_copy_headers(input, gray);

	BMPPIXEL* alt = malloc(img_size * sizeof(BMPPIXEL));
	for (int i = 0; i < img_size; i++) {
		alt[i].B = img[i].B;
		alt[i].G = img[i].B;
		alt[i].R = img[i].B;
	}

	bmp_write_image(gray, alt, img_size);

	// compress it using difference & huffman on bit-length
	VALUE* values = malloc(img_size * sizeof(VALUE));
	differential_compression(values, alt, infoheader.height, infoheader.width);

	FILE* bin = fopen("img/comp.bin", "wb");
	if (bin == NULL) {
		fprintf(stderr, "Error: could not open comp.bin file\n");
		return 1;		
	}

	save_bits(bin, values, img_size);

	VALUE* after = malloc(img_size * sizeof(VALUE));


	fclose(bin);
	bin = fopen("img/comp.bin", "rb");

	load_bits(bin, after);

	// comparing values with after
	for (int i = 0; i < img_size; i++) {
		printf("[%u %u] vs [%u %u]\n", values[i].code, values[i].size, after[i].code, after[i].size);
	}

	free(after);
	free(img);
	free(alt);
	free(values);
	fclose(input);
	fclose(gray);
	fclose(bin);
}