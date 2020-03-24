CC = gcc
FLAGS = -Wall

all: main

main: main.c
	$(CC) $(FLAGS) main.c -lm -o main

comp_test: test.c
	$(CC) $(FLAGS) test.c -lm -o test

test:
	make main
	make comp_test
	./test 100 20 30
	./main lista 10 10 PASTE 1 1
	./test 100 20 30
	./main lista 3 10 NORMAL 1 1
	make clean_all

clean:
	rm -f main
	rm -f test

clean_all:
	make clean
	rm -f A*
	rm -f B*
	rm -f C*
	rm -f lista
