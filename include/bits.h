#ifndef BITS_H
#define BITS_H

#include "utils.h"

/*  */
void load_bits(FILE * fp, INT_PAIR * DC_codes, INT_PAIR * AC_codes, int len_dcs, int * len_acs);

/* Writes symbols in vec into file fp */
void write_bits(INT_PAIR * vec, int len, FILE * fp);

#endif
