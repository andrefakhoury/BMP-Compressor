#ifndef BMP_H
#define BMP_H

#include <stdio.h>

#define HEADER_SIZE 54
#define BF_TYPE 0x4D42 /* "MB" */

typedef struct {
	unsigned short type;
	unsigned int size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int offbits;
} BMPFILEHEADER;

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

typedef struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
} BMPPIXEL;

void bmp_load_file_header(FILE* fp, BMPFILEHEADER* fileheader);
void bmp_load_info_header(FILE* fp, BMPINFOHEADER* infoheader);
void bmp_load_headers(FILE* fp, BMPFILEHEADER* fileheader, BMPINFOHEADER* infoheader);
void bmp_print_headers(FILE* fp, BMPFILEHEADER* fileheader, BMPINFOHEADER* infoheader);
void bmp_copy_headers(FILE* src, FILE* dest);
void bmp_load_image(FILE* fp, BMPPIXEL* image, int img_size);
void bmp_write_image(FILE* fp, BMPPIXEL* image, int img_size);

#endif
