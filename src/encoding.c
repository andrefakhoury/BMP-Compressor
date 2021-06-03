#include <stdlib.h>
#include <stdio.h>
#include "encoding.h"

// Computa a diferença entre dois valores adjacentes
VALUE get_difference(unsigned char cur, unsigned char last) {
	VALUE ret; // resultado

	int diff = (int) cur - last; // calcula a diferença
	ret.code = abs(diff);

	// calcula o tamanho do código
	if (diff == 0) {
		ret.size = 0;
	} else {
		ret.size = sizeof(diff) * 8 - __builtin_clz(ret.code);
	}

	// se a diferença for negativa, aplica complemento de 1
	if (diff < 0) { 
		ret.code = ((~ret.code) << __builtin_clz(ret.code)) >> __builtin_clz(ret.code);
	}

	return ret;
}

// Calcula o array de pares (código, tamanho_código) da codificação de diferenças de img
void differential_compression(RGBPIXEL* img, int height, int width, VALUE* values) {
	unsigned char last_col = 0; // variável auxiliar que armazena o valor da coluna anterior
	unsigned char last_row = 0; // variável auxiliar que armazena o valor da linha anterior

	// Computa o array de diferencas
	// Se é o primeiro da linha, faz a diferença com a célula acima, senao com a célula da esquerda
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			values[i * width + j] = get_difference(img[i * width + j].G, j == 0? last_row : last_col);

			// atualiza os valores de last
			if(j == 0) {
				last_row = img[i * width + j].G;
			}
			last_col = img[i * width + j].G;
		}
	}
}

// função auxiliar que concatena "num_bits" em um buffer "src" com valores "value"
unsigned int append_bits(unsigned int src, unsigned char num_bits, unsigned int value) {
	return (src << num_bits) | value;
}

// Escreve o array de valores em um arquivo
void write_bits(VALUE* values, int size, FILE* fp) {
	unsigned int buffer = 0; // buffer de bits
	unsigned char buffer_size = 0; // numero de bits no buffer

	// Insere os bits de values no arquivo
	for (int i = 0; i < size; i++) {
		// "cur" é o codigo final (prefixo + diferença)
		unsigned int cur = append_bits(prefTable[values[i].size], values[i].size, values[i].code);

		// "size" é o numero de bits em "cur"
		unsigned char size = values[i].size + prefSize[values[i].size];

		// Se cur "transbordar" o buffer, adiciona so o pedaço que cabe 
		if (size + buffer_size > sizeof(buffer) * 8) {
			unsigned char extra = size + buffer_size - sizeof(buffer) * 8; // tamanho do pedaço extra
			unsigned int aux = cur & ((1 << extra) - 1); // parte "extra"

			cur = cur >> extra; // atualiza cur com o pedaço que cabe
			buffer = append_bits(buffer, size - extra, cur); // adiciona cur no buffer
			fwrite(&buffer, sizeof(unsigned int), 1, fp); // como o buffer esta cheio, escreve ele no arquivo

			// buffer agora esta vazio
			buffer = 0; 
			buffer_size = 0;
			// coloca o pedaço extra de volta em cur
			cur = aux; 
			size = extra;
		}

		buffer_size += size; // atualiza o tamanho do buffer
		buffer = append_bits(buffer, size, cur); // adiciona cur no buffer
	}

	// Se restou alguma coisa no buffer no final, escreve no arquivo
	if (buffer_size > 0) {
		// preenche espaço extra com 111111... (porque não há nenhum prefixo só com 1s)
		unsigned int extra = sizeof(buffer) * 8 - buffer_size; // espaço extra na esquerda de buffer
		buffer = buffer << extra; // traz o conteúdo de buffer pra esquerda 
		buffer ^= (1u << extra) - 1; // preenche o resto com 1s
		fwrite(&buffer, sizeof(unsigned int), 1, fp);
	}
}

// Carrega os bits do arquivo. Coloca o resultado em values
void load_bits(FILE* fp, VALUE* values) {
	unsigned int buffer = 0; // buffer de bits
	unsigned int buffer_size = 0; // numero de bits no buffer
	unsigned int cur = 0; // byte
	unsigned int cur_pos = 0; // posicao atual no array de values

	int value_size = -1; // tamanho da segunda parte da codificação de diferença. -1 significa "desconhecido"

	// Le os bytes do arquivo
	while(fread(&cur, sizeof(unsigned int), 1, fp) == 1) {

		// passa por todos os bits do byte lido
		for (int bit = sizeof(buffer) * 8 - 1; bit >= 0; bit--) {
			buffer = (buffer << 1) | ((cur >> bit) & 1); // adiciona o bit atual no buffer
			buffer_size++;

			if (value_size == -1) {
				// ve se o buffer atual casa com algum prefixo
				for (int i = 0; i < QT_PREF; i++) {
					if (prefTable[i] == buffer && prefSize[i] == buffer_size) {
						value_size = i; // se achou, agora o tamanho da segunda parte é conhecido
					}
				}

				if (value_size != -1) { // se achou o tamanho da proxima parte, limpa o buffer
					buffer = 0;
					buffer_size = 0;
				}
			}

			if (value_size != -1 && buffer_size == value_size) { // se estamos lendo a segunda parte e terminamos de ler todos os bits
				// adiciona isso no array de values
				values[cur_pos].size = value_size;
				values[cur_pos].code = buffer;
				cur_pos++;

				// limpa o buffer e volta a procurar por um prefixo
				buffer = 0;
				buffer_size = 0;
				value_size = -1;
			}
		}
	}
}
