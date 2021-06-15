#include <stdlib.h>
#include <stdio.h>

#include "differential.h"

void differential_encode(int *vec, int len, INT_PAIR * ans) {
	int last = 0; // last value read
	for(int i = 0; i < len; i++) {
		int value = vec[i] - last; // difference
		ans[i].first = value >= 0? value : ones_complement(-value); // if value is negative, get 1's complement
		ans[i].second = number_of_bits(abs(value)); // size of symbol (in bits)
		last = vec[i];
	}
}

void differential_decode(INT_PAIR *vec, int len, int * ans) {
	int cur = 0; // current prefix sum
	for(int i = 0; i < len; i++) {
		int value;

		if(vec[i].second == 0) value = 0;
		else value = (vec[i].first >> (vec[i].second-1)) ? // if most significant bit is non-zero
				vec[i].first :     // use the same value
				-ones_complement_with_size(vec[i].first, vec[i].second); // get negative of 1s complement
		cur += value;
		ans[i] = cur;
	}
}

/* Huffman prefixes */
#define QT_PREF 11
const unsigned char prefTable[QT_PREF+1] = {2, 3, 4, 0, 5, 6, 14, 30, 62, 126, 254};
const unsigned char prefSize[QT_PREF+1] = {3, 3, 3, 2, 3, 3, 4, 5, 6, 7, 8};

void differential_entropy_encode(INT_PAIR * vec, int len, INT_PAIR * ans) {
	for(int i = 0; i < len; i++) {
		// symbol is concatenation of huffman prefix and value
		int symbol = append_bits(prefTable[vec[i].second], vec[i].second, vec[i].first);
		int symbol_size = prefSize[vec[i].second] + vec[i].second;
		ans[i] = make_int_pair(symbol, symbol_size);
	}
}

/**
 * Finds the corresponding huffman prefix matching
 * the "len" least significant bits of "x"
 * Returns a -1 if no match was found */
int find_differential_prefix(int x, int len) {
	for(int i = 0; i < QT_PREF; i++) {
		if(x == prefTable[i] && prefSize[i] == len) return i;
	}
	return -1;
}
