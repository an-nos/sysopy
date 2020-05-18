CC = gcc
FLAGS = -Wall

all: main

main: main.c
	$(CC) $(FLAGS) -o main main.c -lpthread -lm

test:
	make main
	./main 5 10
	make clean

clean:
	rm -f main
