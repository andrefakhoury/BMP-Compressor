/* ==============================
 * == André Fakhoury            *
 * == Lucas Turci               *
 * ==== SCC0561 - Multimídia    *
 * ==== Prof. Rudinei           *
 * == ICMC - 2021               *
 * =============================*/

#include "bmp.h"
#include "compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void grayscale(BMPPIXEL *image, int img_size, FILE *input) {
	// abre o arquivo da imagem em tons de cinza
	FILE *gray = fopen("cinza.bmp", "wb");
	if (gray == NULL) {
		fprintf(stderr, "Error: could not open ""cinza"" file\n");
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

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: %s <imagem_entrada>\n", argv[0]);
		return 0;
	}
	
	// Abre arquivo de entrada
	FILE* input = fopen(argv[1], "rb");
	if (input == NULL) {
		fprintf(stderr, "Error: could not open input file\n");
		return 1;
	}

	BMPFILEHEADER fileheader; /* File header */
	BMPINFOHEADER infoheader; /* Info header */

	// Carrega headers nas estruturas
	bmp_load_headers(input, &fileheader, &infoheader);

	// Aloca array contendo a imagem
	int img_size = infoheader.width * infoheader.height;
	BMPPIXEL* img = malloc(img_size * sizeof(BMPPIXEL));

	// Carrega a imagem no array
	bmp_load_image(input, img, img_size);

	// Transforma a imagem pra tons de cinza, e escreve em um arquivo
	grayscale(img, img_size, input);

	// Acha os valores a serem escritos no arquivo de saida (VALUE = TABELA)
	VALUE* values = differential_compression(img, infoheader.height, infoheader.width);

	// Abre o arquivo de saida
	FILE* output = fopen("out.bin", "wb");
	if (output == NULL) {
		fprintf(stderr, "Error: could not open ""out.bin"" file\n");
		return 1;		
	}

	// Copia os headers pro arquivo de saida
	bmp_copy_headers(input, output);

	// Escreve os bits no arquivo de saida
	write_bits(values, img_size, output);

	fclose(output);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* EXTRA: pra certificar que nosso algoritmo funciona, vamos ler o resultado e comparar os valores interpretados com os que foram escritos */
	VALUE* after = malloc(img_size * sizeof(VALUE));

	// Abre o arquivo escrito novamente, agora no modo de leitura
	output = fopen("out.bin", "rb");

	// Le os headers
	bmp_load_headers(output, &fileheader, &infoheader);

	// Carrega os bits no array de VALUES, "after"
	load_bits(output, after);

	// Compara valores originais com valores interpretados
	for (int i = 0; i < img_size; i++) {
		assert(values[i].code == after[i].code && values[i].size == after[i].size);
		//printf("[%u %u] vs [%u %u]\n", values[i].code, values[i].size, after[i].code, after[i].size);
	}

	free(after);
	free(img);
	free(values);
	fclose(input);
	fclose(output);
}