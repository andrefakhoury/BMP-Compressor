#ifndef PREPARATION_H
#define PREPARATION_H

#include "bmp.h"

/** Padding of a matrix with zeros */
void padding(const double *v, double *dest, const int old_w, const int new_w, const int old_h, const int new_h);

/** Convert 'size' RGB pixels to YCbCr */
void rgb_to_ycbcr(const RGBPIXEL *rgb, double *Y, double *Cb, double *Cr, const int size);

/** Convert YCbCr pixels to RGB, storing the result on image */
void ycbcr_to_rgb(const double *Y, const double *Cb, const double *Cr, RGBPIXEL *image, const int size);

/** Subsampling 4:2:0 of v to dest */
void subsample(const double* v, double* dest, const int width, const int height);

/** Reverse process of subsampling. */
void reverse_subsample(const double* s, double* dest, const int width, const int height);

/** Divide channel v in blocks of size 8x8. Assuming BLOCKS is already alloc'd with size [(width/8)*(height/8)][64] */
void divide_in_blocks(double* v, double** blocks, const int width, const int height);

/** Merge back blocks in block to channel V. Assuming everything is alloc'd */
void merge_blocks(double** blocks, double* v, const int width, const int height);

#endif
