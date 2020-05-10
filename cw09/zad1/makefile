CC = gcc
FLAGS = -Wall

all: main

main: main.c
	$(CC) $(FLAGS) -o main main.c -lpthread -lm

test:
	make main

clean:
	rm -f main
