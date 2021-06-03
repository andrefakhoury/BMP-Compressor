#include "utils.h"

void grayscale(RGBPIXEL *image, int img_size, FILE *input) {
	// abre o arquivo da imagem em tons de cinza
	FILE *gray = fopen("cinza.bmp", "wb");
	if (gray == NULL) {
		fprintf(stderr, "Error: could not open \"cinza\" file\n");
		exit(1);
	}

	// copia os headers da imagem original para a em tons de cinza
	bmp_copy_headers(input, gray);

	// transforma a imagem
	for (int i = 0; i < img_size; i++) {
		image[i].B = image[i].G;
		image[i].R = image[i].G;
	}

	// escreve a imagem no arquivo
	bmp_write_image(gray, image, img_size);

	fclose(gray);
}
