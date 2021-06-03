#include "bmp.h"
#include "utils.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
	RGBPIXEL* img = malloc(img_size * sizeof(RGBPIXEL));

	// Carrega a imagem no array
	bmp_load_image(input, img, img_size);

	// Confere se o tamanho é múltiplo de 8
	assert(infoheader.width % 8 == 0 && infoheader.height % 8 == 0);

	//////////////////////////////////////////////////////////////////////////////
	/// Image & Block preparation

	//////////////////////////////////////////////////////////////////////////////
	/// DCT & Quantization

	//////////////////////////////////////////////////////////////////////////////
	/// Encoding
	VALUE *values = malloc(img_size * sizeof(VALUE)); // aloca o array resultado

	// Acha os valores a serem escritos no arquivo de saida (VALUE = TABELA)
	differential_compression(img, infoheader.height, infoheader.width, values);

	// Abre o arquivo de saida
	FILE* output = fopen("out.bin", "wb");
	if (output == NULL) {
		fprintf(stderr, "Error: could not open \"out.bin\" file\n");
		return 1;
	}

	// Copia os headers pro arquivo de saida
	bmp_copy_headers(input, output);

	// Escreve os bits no arquivo de saida
	write_bits(values, img_size, output);

	fclose(output);

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Uncompress

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
