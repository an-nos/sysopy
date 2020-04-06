CC = gcc
FLAGS = -Wall

all: main

main: main.c producer.c consumer.c
	$(CC) $(FLAGS) -o main main.c
	$(CC) $(FLAGS) -o producer producer.c
	$(CC) $(FLAGS) -o consumer consumer.c
clean:
	rm -f main
	rm -f producer
	rm -f consumer
	rm -f pipe
	rm -f prod1
	rm -f prod2
	rm -f prod3
	rm -f prod4
	rm -f prod5
	rm -f cons1


