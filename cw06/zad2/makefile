CC = gcc
FLAGS = -Wall

all: main

main: client.c server.c
	$(CC) $(FLAGS) -o client client.c -lrt
	$(CC) $(FLAGS) -o server server.c -lrt

test:
	make main
	./server

clean:
	rm -f server
	rm -f client



