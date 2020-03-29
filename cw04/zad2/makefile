CC = gcc
FLAGS = -Wall

all: main

main: main.c exec_test.c
	$(CC) $(FLAGS) -o main main.c
	$(CC) $(FLAGS) -o exec_test exec_test.c

clean:
	rm -f main
	rm -f exec_test