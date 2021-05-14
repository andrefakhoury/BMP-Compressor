all:
	gcc src/compress.c src/bmp.c src/main.c -o main -I include/ -std=c99 -Wall -fsanitize=address,undefined -g -lm

clean:
	rm main