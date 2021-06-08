#include <string.h>

#include "preparation.h"
#include "utils.h"

/** Padding of a matrix with zeros */
void padding(const double *v, double *dest, const int old_w, const int new_w, const int old_h, const int new_h) {
	memset(dest, 0, new_w * new_h * sizeof(double)); // fill with zeros
	const int min_w = old_w < new_w ? old_w : new_w;
	const int min_h = old_h < new_h ? old_h : new_h;

	for (int i = 0; i < min_h; i++) {
		for (int j = 0; j < min_w; j++) {
			dest[i * new_w + j] = v[i * old_w + j];
		}
	}
}

/** Convert 'size' RGB pixels to YCbCr */
void rgb_to_ycbcr(const RGBPIXEL *rgb, double *Y, double *Cb, double *Cr, const int size) {
    for (int i = 0; i < size; i++) {
        Y[i] = 0.299 * rgb[i].R + 0.587 * rgb[i].G + 0.114 * rgb[i].B;
        Cb[i] = 0.564 * (rgb[i].B - Y[i]);
        Cr[i] = 0.713 * (rgb[i].R - Y[i]);
    }
}

/** Convert YCbCr pixels to RGB, storing the result on image */
void ycbcr_to_rgb(const double *Y, const double *Cb, const double *Cr, RGBPIXEL *image, const int size) {
    for (int i = 0; i < size; i++) {
        image[i].R = clip(Y[i] + 1.402 * Cr[i], 0, 255);
        image[i].G = clip(Y[i] - 0.344 * Cb[i] - 0.714 * Cr[i], 0, 255);
        image[i].B = clip(Y[i] + 1.772 * Cb[i], 0, 255);
    }
}

/** Subsampling 4:2:0 of v to dest */
void subsample(const double* v, double* dest, const int width, const int height) {
	const int new_width = (width + 1) / 2;
	for (int i = 0; i < height; i += 2) {
		for (int j = 0; j < width; j += 2) {
			dest[i / 2 * new_width + j / 2] = v[i * width + j];
		}
	}
}

/** Reverse process of subsampling. */
void reverse_subsample(const double* s, double* dest, const int width, const int height) {
	const int sub_width = (width + 1) / 2;
	const int sub_height = (height + 1) / 2;

	for (int i = 0; i < sub_height; i++) {
		for (int j = 0; j < sub_width; j++) {
			for (int di = 0; di < 2 && 2 * i + di < height; di++) {
				for (int dj = 0; dj < 2 && 2 * j + dj < width; dj++) {
					dest[(2 * i + di) * width + 2 * j + dj] = s[i * sub_width + j];
				}
			}
		}
	}
}

/** Divide channel v in blocks of size 8x8. Assuming BLOCKS is already alloc'd with size [(width/8)*(height/8)][64] */
void divide_in_blocks(double* v, double** blocks, const int width, const int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int cur_block = i / 8 * (width / 8) + j / 8; // block to add
			int cur_pos = (i % 8) * 8 + j % 8; // position on current block
			blocks[cur_block][cur_pos] = v[i * width + j];
		}
	}
}

/** Merge back blocks in block to channel V. Assuming everything is alloc'd */
void merge_blocks(double** blocks, double* v, const int width, const int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int cur_block = i / 8 * (width / 8) + j / 8; // block to add
			int cur_pos = (i % 8) * 8 + j % 8; // position on current block
			v[i * width + j] = blocks[cur_block][cur_pos];
		}
	}
}
