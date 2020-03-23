CC = gcc
FLAGS = -Wall

all: main

main: main.c
	$(CC) $(FLAGS) main.c -lm -o main

test: test.c
	$(CC) $(FLAGS) test.c -lm -o test

clean:
	rm -f main
	rm -f test
