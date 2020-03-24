CC = gcc
FLAGS = -Wall -ggdb3

all: main

main: main.c
	$(CC) $(FLAGS) main.c -lm -o main

comp_test: test.c
	$(CC) $(FLAGS) test.c -lm -o test

test:
	make main
	make comp_test
	./test 20 20 30
	make clean_all

clean:
	rm -f main
	rm -f test

clean_all:
	make clean
	make clean_files

clean_files:
	rm -f A*
	rm -f B*
	rm -f C*
	rm -f lista
