#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_ZEROS 15
#define MAX_NUM_BITS 10
#define MAX_PREFIX_SIZE 16

#define EOB "1010"

char _rle_prefixes[MAX_NUM_ZEROS+1][MAX_NUM_BITS+1][MAX_PREFIX_SIZE + 1];

void read_prefixes() {
	FILE * fp = fopen("rle_prefixes.txt", "r");

	if(!fp) {
		fprintf(stderr, "Could not open file for Run-Length Encoding huffman prefixes");
		exit(1);
	}

	for(int i = 0; i <= MAX_NUM_ZEROS; i++) {
		for(int j = i != MAX_NUM_ZEROS; j <= MAX_NUM_BITS; j++) {
			fscanf(fp, "%s ", _rle_prefixes[i][j]);
		}
	}

	fclose(fp);
}


int main() {
	read_prefixes();
}