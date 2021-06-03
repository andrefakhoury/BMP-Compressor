#ifndef PREPARATION_H
#define PREPARATION_H

#include "bmp.h"

typedef struct {
    float Y, U, V;
} YUVPIXEL;

/** Convert RGB pixels to YUV pixels */
void rgb_to_yuv(RGBPIXEL *image, YUVPIXEL *result, int length);

/** Extract Y, U and V channels from struct YUVPIXEL array */
void extract_channels(YUVPIXEL *image, float *Y, float *U, float *V, int length);

/** Divide current channel 'image' in blocks of size 8x8 */
void divide_in_blocks(float *image, float **result, int width, int height);

#endif
