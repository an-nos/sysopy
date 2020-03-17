CC = gcc
FLAGS = -Wall
LINES = 4000
LEN = 5000

all: main

main: zad1.c
	$(CC) $(FLAGS) -o zad1 zad1.c

test:
	make main
	./zad1 generate test $(LINES) $(LEN)
	./zad1 copy test test_l $(LINES) $(LEN) lib
	./zad1 copy test test_s $(LINES) $(LEN) sys
	./zad1 sort test_l $(LINES) $(LEN) lib
	./zad1 sort test_s $(LINES) $(LEN) sys
	make clean_all


clean:
	rm -f zad1

clean_all:
	make clean
	rm -f test
	rm -f test_l
	rm -f test_s