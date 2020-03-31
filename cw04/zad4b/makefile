CC = gcc
FLAGS = -Wall

all: main

main: catcher.c sender.c
	$(CC) $(FLAGS) -o catcher catcher.c
	$(CC) $(FLAGS) -o sender sender.c

clean:
	rm -f catcher
	rm -f sender