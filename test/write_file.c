#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int first, second;
} INT_PAIR;

INT_PAIR make_int_pair(int a, int b) {
	INT_PAIR ret;
	ret.first = a;
	ret.second = b;
	return ret;
}

// auxiliar function that concatenates "num_bits" in a buffer "src" with value "value"
unsigned int append_bits(unsigned int src, unsigned char num_bits, unsigned int value) {
	return (src << num_bits) | value;
}

void write_bits(INT_PAIR * vec, int len, FILE * fp) {
	unsigned int buffer = 0; // buffer of bits
	int buffer_size = 0; // number of bits inside buffer

	// Insere os bits de values no arquivo
	for (int i = 0; i < len; i++) {

		// value to write
		unsigned int cur = vec[i].first;

		// "size": number of bits in cur
		int size = vec[i].second;

		if (size + buffer_size > sizeof(buffer) * 8) {
			int extra = size + buffer_size - sizeof(buffer) * 8; // tamanho do pedaço extra
			int aux = cur & ((1 << extra) - 1); // parte "extra"

			cur = cur >> extra; // atualiza cur com o pedaço que cabe
			buffer = append_bits(buffer, size - extra, cur); // adiciona cur no buffer
			fwrite(&buffer, sizeof(int), 1, fp); // como o buffer esta cheio, escreve ele no arquivo

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
		int extra = sizeof(buffer) * 8 - buffer_size; // espaço extra na esquerda de buffer
		buffer = buffer << extra; // traz o conteúdo de buffer pra esquerda 
		buffer ^= (1u << extra) - 1; // preenche o resto com 1s
		fwrite(&buffer, sizeof(int), 1, fp);
	}
}

int main() {
	FILE * fp = fopen("test.bin", "wb");

	INT_PAIR vec[] = {
		make_int_pair(9, 4),
		make_int_pair(6, 4),
		make_int_pair(1, 2),
		make_int_pair(26, 5),
		make_int_pair(15, 4),
		make_int_pair(1, 2),
		make_int_pair(0, 3),
	};

	write_bits(vec, 7, fp);

	fclose(fp);
}