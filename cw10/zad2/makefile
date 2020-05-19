CC = gcc
FLAGS = -Wall

all: main

main: client.c server.c
	$(CC) $(FLAGS) -o client client.c -lpthread -lm
	$(CC) $(FLAGS) -o server server.c -lpthread -lm

clean:
	rm -f client
	rm -f server
