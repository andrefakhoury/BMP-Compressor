#ifndef BMP_H
#define BMP_H

#include <stdio.h>

#define FILEHEADER_SIZE 14
#define HEADER_SIZE 54
#define BF_TYPE 0x4D42 /* "MB" */

/** File Header of a BMP Image */
typedef struct {
	unsigned short type;
	unsigned int size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int offbits;
} BMPFILEHEADER;

/** Information Header of a BMP Image */
typedef struct {
	unsigned int size;
	int width;
	int height;
	unsigned short planes;
	unsigned short bitcount;
	unsigned int compression;
	unsigned int sizeimg;
	int xresolution;
	int yresolution;
	unsigned int ncolors;
	unsigned int importantcolors;
} BMPINFOHEADER;

/** RGB Pixel of a BMP Image */
typedef struct {
	unsigned char R, G, B;
} RGBPIXEL;

/** Load the headers of image 'fp' */
void bmp_load_headers(FILE* fp, BMPFILEHEADER* fileheader, BMPINFOHEADER* infoheader);

/** Print the headers fileheader and infoheader on stream 'fp' */
void bmp_print_headers(FILE* fp, BMPFILEHEADER* fileheader, BMPINFOHEADER* infoheader);

/** Copy headers from src to dest */
void bmp_copy_headers(FILE* src, FILE* dest);

/** Load pixels of image file 'fp' of size 'img_size' */
void bmp_load_image(FILE* fp, RGBPIXEL* image, int img_size);

/** Write 'img_size' pixels of image to stream 'fp' */
void bmp_write_image(FILE* fp, RGBPIXEL* image, int img_size);

#endif
