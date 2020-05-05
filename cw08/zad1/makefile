CC = gcc
FLAGS = -Wall

all: main

main: main.c
	$(CC) $(FLAGS) -o main main.c -lpthread -lm

test:
	make main

make times:
	make main
	./main 1 sign apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 2 sign apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 4 sign apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 8 sign apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 1 block apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 2 block apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 4 block apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 8 block apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 1 interleaved apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 2 interleaved apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 4 interleaved apollonian_gasket.ascii.pgm res.txt >> Times.txt
	./main 8 interleaved apollonian_gasket.ascii.pgm res.txt >> Times.txt

clean:
	rm -f main
