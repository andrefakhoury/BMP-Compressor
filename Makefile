all:
	gcc src/*.c -o compress -I include/ -std=c99 -Wall -fsanitize=address,undefined -g -U__STRICT_ANSI__ -lm

clean:
	rm main