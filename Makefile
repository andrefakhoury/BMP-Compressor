all:
	gcc src/*.c -o main.out -I include/ -std=c99 -Wall -fsanitize=address,undefined -g

clean:
	rm main