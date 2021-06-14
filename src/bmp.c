#include "bmp.h"
#include <stdio.h>

void bmp_load_file_header(FILE* fp, BMPFILEHEADER* fileheader) {
	// seek to begin of file
	fseek(fp, 0, SEEK_SET);

	if(fileheader == NULL) return;
	
	// read each field
	fread(&fileheader->type, sizeof(unsigned short), 1, fp);
	fread(&fileheader->size, sizeof(unsigned int), 1, fp);
	fread(&fileheader->reserved1, sizeof(unsigned short), 1, fp);
	fread(&fileheader->reserved2, sizeof(unsigned short), 1, fp);
	fread(&fileheader->offbits, sizeof(unsigned int), 1, fp);
}

void bmp_load_info_header(FILE* fp, BMPINFOHEADER* infoheader) {
	// seek to first information bit
	fseek(fp, FILEHEADER_SIZE, SEEK_SET);

	if(infoheader == NULL) return;

	// read each field
	fread(&infoheader->size, sizeof(unsigned int), 1, fp);
	fread(&infoheader->width, sizeof(int), 1, fp);
	fread(&infoheader->height, sizeof(int), 1, fp);
	fread(&infoheader->planes, sizeof(unsigned short), 1, fp);
	fread(&infoheader->bitcount, sizeof(unsigned short), 1, fp);
	fread(&infoheader->compression, sizeof(unsigned int), 1, fp);
	fread(&infoheader->sizeimg, sizeof(unsigned int), 1, fp);
	fread(&infoheader->xresolution, sizeof(int), 1, fp);
	fread(&infoheader->yresolution, sizeof(int), 1, fp);
	fread(&infoheader->ncolors, sizeof(unsigned int), 1, fp);
	fread(&infoheader->importantcolors, sizeof(unsigned int), 1, fp);
}

void bmp_load_headers(FILE* fp, BMPFILEHEADER* fileheader, BMPINFOHEADER* infoheader) {
	// load file and info headers
	bmp_load_file_header(fp, fileheader);
	bmp_load_info_header(fp, infoheader);
}

void bmp_print_headers(FILE* fp, BMPFILEHEADER* fileheader, BMPINFOHEADER* infoheader) {
	// print each header value (used for debugging)

	fprintf(fp, "== File Header ==\n");
	fprintf(fp, "type: %x\n", fileheader->type);
	fprintf(fp, "size: %d\n", fileheader->size);
	fprintf(fp, "reserved1: %hu\n", fileheader->reserved1);
	fprintf(fp, "reserved2: %hu\n", fileheader->reserved2);
	fprintf(fp, "offbits: %d\n", fileheader->offbits);

	fprintf(fp, "== Info Header ==\n");
	fprintf(fp, "size: %d\n", infoheader->size);
	fprintf(fp, "width: %d\n", infoheader->width);
	fprintf(fp, "height: %d\n", infoheader->height);
	fprintf(fp, "planes: %hu\n", infoheader->planes);
	fprintf(fp, "bitcount: %hu\n", infoheader->bitcount);
	fprintf(fp, "compression: %d\n", infoheader->compression);
	fprintf(fp, "sizeimg: %d\n", infoheader->sizeimg);
	fprintf(fp, "xresolution: %d\n", infoheader->xresolution);
	fprintf(fp, "yresolution: %d\n", infoheader->yresolution);
	fprintf(fp, "ncolors: %d\n", infoheader->ncolors);
	fprintf(fp, "importantcolors: %d\n", infoheader->importantcolors);
}

void bmp_copy_headers(FILE* src, FILE* dest) {
	// seek both pointers to begin of file
	fseek(src, 0, SEEK_SET);
	fseek(dest, 0, SEEK_SET);

	// read from src and write into dest
	for (int i = 0; i < HEADER_SIZE; i++) {
		fputc(fgetc(src), dest);
	}
}

void bmp_load_image(FILE* fp, RGBPIXEL* image, int img_size) {
	// seek to HEADER_SIZE (position of first pixel)
	fseek(fp, HEADER_SIZE, SEEK_SET);

	// read each pixel info
	for (int i = 0; i < img_size; i++) {
		image[i].B = fgetc(fp);
		image[i].G = fgetc(fp);
		image[i].R = fgetc(fp);
	}
}

void bmp_write_image(FILE* fp, RGBPIXEL* image, int img_size) {
	// seek to HEADER_SIZE (position of first pixel)
	fseek(fp, HEADER_SIZE, SEEK_SET);

	// print each pixel info
	for (int i = 0; i < img_size; i++) {
		fputc(image[i].B, fp);
		fputc(image[i].G, fp);
		fputc(image[i].R, fp);
	}
}
