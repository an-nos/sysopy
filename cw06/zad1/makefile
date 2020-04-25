CC = gcc
FLAGS = -Wall

all: main

main: client.c server.c
	$(CC) $(FLAGS) -o client client.c
	$(CC) $(FLAGS) -o server server.c

test:
	make main
	./server

clean:
	rm -f server
	rm -f client
