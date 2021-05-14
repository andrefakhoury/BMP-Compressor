#include <stdlib.h>
#include <stdio.h>
#include "compress.h"

VALUE get_difference(unsigned char cur, unsigned char last) {
	VALUE ret;

	int diff = (int) cur - last;
	ret.code = abs(diff);
	if (diff == 0) {
		ret.size = 0;
	} else {
		ret.size = sizeof(diff) * 8 - __builtin_clz(ret.code);
	}

	if (diff < 0) { // apply 1-complement
		ret.code = ((~ret.code) << __builtin_clz(ret.code)) >> __builtin_clz(ret.code);
	}

	return ret;
}

void differential_compression(VALUE* values, BMPPIXEL* img, int height, int width) {
	unsigned char last = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			values[i * width + j] = get_difference(img[i * width + j].G, last);
			last = img[i * width + j].G;
		}
	}
}

unsigned int append_bits(unsigned int src, unsigned char qtt_bits, unsigned int value) {
	return (src << qtt_bits) | value;
}

void save_bits(FILE* fp, VALUE* values, int size) {
	unsigned int carry = 0;
	unsigned char carry_size = 0;
	for (int i = 0; i < size; i++) {
		unsigned int cur = append_bits(prefTable[values[i].size], values[i].size, values[i].code);
		unsigned char size = values[i].size + prefSize[values[i].size];

		if (size + carry_size > sizeof(carry) * 8) {
			unsigned char extra = size + carry_size - sizeof(carry) * 8;
			unsigned int aux = cur & ((1 << extra) - 1);

			cur = cur >> extra;
			carry = append_bits(carry, size - extra, cur);
			fwrite(&carry, sizeof(unsigned int), 1, fp);

			carry = 0;
			carry_size = 0;
			cur = aux;
			size = extra;
		}

		carry_size += size;
		carry = append_bits(carry, size, cur);
	}

	if (carry_size > 0) {
		// fill extra space with 111111... (because there is no prefix starting with 1111...11)
		unsigned int extra = sizeof(carry) * 8 - carry_size;
		carry = carry << extra;
		carry ^= (1 << extra) - 1;
		fwrite(&carry, sizeof(unsigned int), 1, fp);
	}
}

void load_bits(FILE* fp, VALUE* values) {
	unsigned int carry = 0, carry_size = 0, cur = 0, cur_id = 0;

	int nx_size = -1;
	while(fread(&cur, sizeof(unsigned int), 1, fp) == 1) {
		for (int bit = sizeof(carry) * 8 - 1; bit >= 0; bit--) {
			carry = (carry << 1) | ((cur >> bit) & 1);
			carry_size++;

			if (nx_size == -1) {
				for (int i = 0; i < QT_PREF; i++) {
					if (prefTable[i] == carry && prefSize[i] == carry_size) {
						nx_size = i;
					}
				}

				if (nx_size != -1) {
					carry = 0;
					carry_size = 0;
				}
			}

			if (nx_size != -1 && carry_size == nx_size) {
				values[cur_id].size = nx_size;
				values[cur_id].code = carry;
				cur_id++;

				carry = 0;
				carry_size = 0;
				nx_size = -1;
			}
		}
	}
}
