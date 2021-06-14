#include <stdlib.h>
#include <stdio.h>

#include "differential.h"

void differential_encode(int * vec, int len, INT_PAIR * ans) {
	int last = 0;
	for(int i = 0; i < len; i++) {
		int value = vec[i] - last;
		ans[i].first = value >= 0? value : ones_complement(-value);
		ans[i].second = number_of_bits(abs(value));
		last = vec[i];
	}
}

void differential_decode(INT_PAIR * vec, int len, int * ans) {
	int cur = 0;
	for(int i = 0; i < len; i++) {
		int value;
		if(vec[i].second == 0) value = 0;
		else value = (vec[i].first >> (vec[i].second-1))? vec[i].first : -ones_complement_with_size(vec[i].first, vec[i].second);
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
		int symbol = append_bits(prefTable[vec[i].second], vec[i].second, vec[i].first);
		int symbol_size = prefSize[vec[i].second] + vec[i].second;
	
		ans[i] = make_int_pair(symbol, symbol_size);
	}
}

// Finds the corresponding huffman prefix matching
// the "len" least significant bits of "x"
// Returns a -1 if no match was found
int find_differential_prefix(int x, int len) {
	for(int i = 0; i < QT_PREF; i++) {
		if(x == prefTable[i] && prefSize[i] == len) return i;
	}
	return -1;
}

void differential_entropy_decode(INT_PAIR * vec, int len, INT_PAIR * ans) {
	for(int i = 0; i < len; i++) {
		int cur = 0, cur_sz = 0;
		ans[i].second = -1;

		for(int j = vec[i].second-1; j >= 0; --j) {
			cur <<= 1;
			cur |= (vec[i].first >> j)&1;
			cur_sz++;

			int symbol_size = find_differential_prefix(cur, cur_sz);
			if(symbol_size != -1 && ans[i].second == -1) {
				ans[i].second = symbol_size;
				cur = cur_sz = 0;
			}

		}
		ans[i].first = cur;
	}
}


/* 
int main() {

	int aux = 9;
	printf("%d %d %d\n", aux, (~aux)<<1, ones_complement_with_size(aux, 5));

	// diffs =  {1, 0, 32, 178, -22, -1, 4, 7, 12, -16, 0, 1}
	int vec[] = {1, 1, 33, 211, 189, 188, 192, 199, 211, 195, 195, 196};
	int res[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int len = 12;

	INT_PAIR * code = (INT_PAIR *) malloc(sizeof(INT_PAIR) * len);

	differential_encode(vec, len, code);

	for(int i = 0; i < len; ++i) {
		printf("(%d, %d)\n", code[i].first, code[i].second);
	}

	INT_PAIR * code2 = (INT_PAIR *) malloc(sizeof(INT_PAIR) * len);

	differential_entropy_encode(code, len, code2);

	printf("-------------------------\n");
	for(int i = 0; i < len; ++i) {
		printf("(%d, %d)\n", code2[i].first, code2[i].second);
	}

	INT_PAIR * res0 = (INT_PAIR *) malloc(sizeof(INT_PAIR) * len);

	differential_entropy_decode(code2, len, res0);

	differential_decode(res0, len, res);
	for(int i = 0; i < len; i++) printf("%d ", res[i]);
	printf("\n");
} */