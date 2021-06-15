SRC=src/bmp.c src/differential.c src/encoding.c src/rle.c src/utils.c src/bits.c src/preparation.c src/dct.c
MAIN_COMPRESS=src/compress.c
MAIN_DECOMPRESS=src/decompress.c
CFLAGS=-O2 -static -Wno-unused-result -Wall -std=c99 -U__STRICT_ANSI__ -lm

compress: $(SRC) $(MAIN_COMPRESS) include/*
	gcc $(SRC) $(MAIN_COMPRESS) -o compress -I include/ $(CFLAGS)

decompress: $(SRC) $(MAIN_DECOMPRESS) include/*
	gcc $(SRC) $(MAIN_DECOMPRESS) -o decompress -I include/ $(CFLAGS)

.PHONY: clean

clean:
	rm compress decompress