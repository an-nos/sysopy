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

clean_all:
	make clean
	rm -f A*
	rm -f B*
	rm -f C*
	rm -f lista
