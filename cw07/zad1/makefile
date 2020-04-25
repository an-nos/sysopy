CC = gcc
FLAGS = -Wall

all: main

main: main.c worker1.c worker2.c worker3.c
	$(CC) $(FLAGS) -o main main.c
	$(CC) $(FLAGS) -o worker1 worker1.c
	$(CC) $(FLAGS) -o worker2 worker2.c
	$(CC) $(FLAGS) -o worker3 worker3.c


test:
	make main
	./main

clean:
	rm -f main
	rm -f worker1
	rm -f worker2
	rm -f worker3

