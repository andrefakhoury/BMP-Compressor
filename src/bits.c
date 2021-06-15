#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bits.h"
#include "differential.h"
#include "rle.h"

void load_bits(FILE * fp, INT_PAIR * DC_codes, INT_PAIR * AC_codes, int len_dcs, int * len_acs) {
	int reader = 0; // current bits to read
	int reader_size = 0; // number of bits in reader

	int buffer = 0; // buffer to store the bits read
	int buffer_size = 0; // number of bits in the buffer

	// first, read DCs
	int cnt_read = 0;
	int symbol_size = -1;
	while(cnt_read < len_dcs) {		
		if(reader_size == 0) {
			fread(&reader, sizeof(reader), 1, fp);
			reader_size = sizeof(reader) * 8;
		}
		// read a bit from the reader
		buffer <<= 1;
		buffer |= (reader >> (reader_size - 1)) & 1;
		reader_size--;
		buffer_size++;

		if(buffer_size + 1 == sizeof(buffer) * 8) { // if I read one more time, its going to overflow
			fprintf(stderr, "Something wrong happened during the decoding\n");
			exit(-1);
		}

		// if reading the prefix
		if(symbol_size == -1 && find_differential_prefix(buffer, buffer_size) != -1) {
			symbol_size = find_differential_prefix(buffer, buffer_size);
			buffer_size = 0;
			buffer = 0;
		}

		// if reading the symbol
		if(symbol_size != -1 && buffer_size == symbol_size) {
			DC_codes[cnt_read++] = make_int_pair(buffer, buffer_size);
			symbol_size = -1;
			buffer_size = 0;
			buffer = 0;
		}
	}

	assert(symbol_size == -1);
	assert(buffer == 0 && buffer_size == 0);

	cnt_read = 0;
	int zeros;

	// Now, read ACs
	while(1) {
		if(reader_size == 0) {
			fread(&reader, sizeof(reader), 1, fp);
			reader_size = sizeof(reader) * 8;
		}
		// read a bit from the reader
		buffer <<= 1;
		buffer |= (reader >> (reader_size - 1)) & 1;
		reader_size--;
		buffer_size++;

		if(buffer_size + 1 == sizeof(buffer) * 8) { // if I read one more time, its going to overflow
			fprintf(stderr, "Something wrong happened during the decoding\n");
			exit(-1);
		}
 
		INT_PAIR ret = find_rle_prefix(buffer, buffer_size);
		
		// if reading the prefix
		if(symbol_size == -1 && ret.first != -1) {
			zeros = ret.first;
			symbol_size = ret.second;

			if(ret.second + ret.first == 0) { // flag for end of block
				break;
			}
			buffer_size = 0;
			buffer = 0;
		}
		if(symbol_size != -1 && buffer_size == symbol_size) {
			if(buffer_size && (buffer >> (buffer_size-1)) == 0) buffer = -ones_complement_with_size(buffer, buffer_size);
			AC_codes[cnt_read++] = make_int_pair(zeros, buffer);
			buffer = 0;
			buffer_size = 0;
			symbol_size = -1;
		}
	}

	*len_acs = cnt_read;

	assert(ones_complement_with_size(reader, reader_size) == 0);
}

void write_bits(INT_PAIR * vec, int len, FILE * fp) {
	unsigned int buffer = 0; // buffer of bits
	int buffer_size = 0; // number of bits inside buffer

	// Insert bits of values into file
	for (int i = 0; i < len; i++) {

		// value to write
		unsigned int cur = vec[i].first;

		// "size": number of bits in cur
		int size = vec[i].second;

		if (size + buffer_size > sizeof(buffer) * 8) {
			int extra = size + buffer_size - sizeof(buffer) * 8; // extra size
			int aux = cur & ((1 << extra) - 1); // extra bits in buffer

			cur = cur >> extra; // update cur with extra
			buffer = append_bits(buffer, size - extra, cur); // add cur to buffer
			fwrite(&buffer, sizeof(int), 1, fp); // buffer is full, write to file

			// buffer is already used, so it's empty now
			buffer = 0; 
			buffer_size = 0;
			// update extra back to cur
			cur = aux; 
			size = extra;
		}

		buffer_size += size; // update buffer size
		buffer = append_bits(buffer, size, cur); // add cur to buffer
	}

	// if there is any values after the main loop, write it to file
	if (buffer_size > 0) {
		// fill extra space with 111111... (unused value)
		int extra = sizeof(buffer) * 8 - buffer_size;
		buffer = buffer << extra;
		buffer ^= (1u << extra) - 1;
		fwrite(&buffer, sizeof(int), 1, fp);
	}
}
