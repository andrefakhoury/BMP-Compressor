#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rle.h"

#define MAX_NUM_ZEROS 15
#define MAX_NUM_BITS 10
#define MAX_PREFIX_SIZE 16
#define MAX_PREFIX_VALUE 65535

// RLE Table of prefixes (Tabela 4.3 do pdf da Poli sobre JPEG)
INT_PAIR _rle_prefixes[MAX_NUM_ZEROS+1][MAX_NUM_BITS+1];
INT_PAIR _rle_prefix_map[MAX_PREFIX_SIZE+1][MAX_PREFIX_VALUE+1];

/** convert a string of 0s and 1s into an integer */
int binary_string_to_int(char * str) {
	int ans = 0, n = strlen(str);
	for(int i = 0; i < n; i++) {
		ans <<= 1;
		ans |= str[i] - '0';
	}
	return ans;
}

/** Load RLE Huffman prefixes from file */
void read_rle_prefixes() {
	FILE * fp = fopen("rle_prefixes.txt", "r"); // open file with prefixes

	if(!fp) {
		fprintf(stderr, "Could not open file for Run-Length Encoding huffman prefixes");
		exit(1);
	}

	// Read the prefix strings
	for(int i = 0; i <= MAX_NUM_ZEROS; i++) {
		for(int j = i != MAX_NUM_ZEROS && i; j <= MAX_NUM_BITS; j++) {
			char aux[MAX_PREFIX_SIZE + 1];
			fscanf(fp, "%s ", aux);
			_rle_prefixes[i][j] = make_int_pair(binary_string_to_int(aux), strlen(aux));
			_rle_prefix_map[_rle_prefixes[i][j].second][_rle_prefixes[i][j].first] = make_int_pair(1 + i, 1 + j);
		}
	}

	fclose(fp);
}

/** 
* Encodes a sequence of values with Run Length Encoding, extracting the pairs (number of zeros, value)
* ans must be allocated with at least len * sizeof(INT_PAIR) bytes
* len must be the block size
*/
void rle_encode(int * vec, int len, INT_PAIR * ans, int * ans_len) {
	(*ans_len) = 0;
	INT_PAIR block_end = {}; // flag that indicates a suffix of only zeros

	for(int r, l = -1; l < len; l = r) {
		r = l+1;

		// go over all the zeros until next non-zero. Stop if it achieves bound
		while(r < len && r - l - 1 < MAX_NUM_ZEROS && vec[r] == 0) {
			r++;
		}

		// add next pair
		ans[(*ans_len)++] = make_int_pair(r - l - 1, r == len? 0 : vec[r]);
	}

	// if suffix only contain zeros, pop everything
	while((*ans_len) && ans[(*ans_len)-1].second == 0) (*ans_len)--;

	// put block_end flag, indicating the end of this block
	ans[(*ans_len)++] = block_end;
}

/**
* Decodes a sequence of RLE pairs into it's original array
* It fills the array with zeros until 'cur' is 'ans_len'
*/
void rle_decode(INT_PAIR * vec, int len, int * ans, int ans_len) {
	int cur = 0;
	for(int i = 0; i < len; i++) {
		for(int j = 0; j < vec[i].first; j++) {
			ans[cur++] = 0;
		}
		ans[cur++] = vec[i].second;
	}

	// Fill the rest with zeros
	while(cur < ans_len) {
		ans[cur++] = 0;
	}
}

/** 
* Finds the corresponding pair for the huffman prefix matching 
* the "len" least significant bits of "x"
* Returns a pair of (-1, -1) if no match was found
*/
INT_PAIR find_rle_prefix(int x, int len) {
	int pref = x & ((1 << len)-1);
	assert(pref >= 0 && pref <= MAX_PREFIX_VALUE);
	if(pref >= 0 && pref <= MAX_PREFIX_VALUE && _rle_prefix_map[len][pref].second) 
		return make_int_pair(_rle_prefix_map[len][pref].first - 1, _rle_prefix_map[len][pref].second-1);
	return make_int_pair(-1, -1);
}

/** 
* Encodes a sequence of INT_PAIR of size len with RLE huffman prefixes
* ans must be allocated with at least len * sizeof(INT_PAIR) byte
* ans[i].first stores the symbol and ans[i].second stores the symbol's size
*/
void rle_entropy_encode(INT_PAIR * vec, int len, INT_PAIR * ans) {

	for(int i = 0; i < len; i++) {
		int value = vec[i].second >= 0? vec[i].second : ones_complement(-vec[i].second);
		int size = number_of_bits(abs(vec[i].second));

		int prefix = _rle_prefixes[vec[i].first][size].first;
		ans[i].first = append_bits(prefix, size, value); // prefix + value
		ans[i].second = _rle_prefixes[vec[i].first][size].second + size; // number of bits of symbol
	}
}
