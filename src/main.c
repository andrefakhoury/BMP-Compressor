#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	FILE* input = fopen("img/colors.bmp", "rb");
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

	FILE* output = fopen("img/out.bmp", "wb");
	if (output == NULL) {
		fprintf(stderr, "Error: could not open output file\n");
		return 1;
	}

	bmp_copy_headers(input, output);

	BMPPIXEL* alt = malloc(img_size * sizeof(BMPPIXEL));
	for (int i = 0; i < img_size; i++) {
		alt[i].B = img[i].B;
		alt[i].G = img[i].R;
		alt[i].R = img[i].G;
	}

	bmp_write_image(output, alt, img_size);

	free(img);
	free(alt);
	fclose(input);
	fclose(output);
}