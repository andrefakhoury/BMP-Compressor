SRC=src/bmp.c src/differential.c src/encoding.c src/rle.c src/utils.c src/bits.c src/preparation.c src/dct.c 
MAIN_COMPRESS=src/compress.c
MAIN_DECOMPRESS=src/decompress.c

compress: $(SRC) $(MAIN_COMPRESS) include/*
	gcc $(SRC) $(MAIN_COMPRESS) -o compress -I include/ -std=c99 -Wall -fsanitize=address,undefined -g -U__STRICT_ANSI__ -lm
decompress: $(SRC) $(MAIN_DECOMPRESS) include/*
	gcc $(SRC) $(MAIN_DECOMPRESS) -o decompress -I include/ -std=c99 -Wall -fsanitize=address,undefined -g -U__STRICT_ANSI__ -lm
clean:
	rm compress decompress