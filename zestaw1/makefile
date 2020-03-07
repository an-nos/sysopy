CC = gcc

all: static shared clean

static: library.c
	$(CC) -c library.c
	ar rcs library.a library.o

shared: library.c
	$(CC) -fPIC -c library.c
	$(CC) -fPIC -shared -o library.so library.o

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f library


2static: main.c
	make static
	$(CC) -c main.c
	$(CC) main.o library.a -o main

2shared: main.c
	make shared
	$(CC) main.c library.so -L ./ -o main -Wl,-rpath,$(shell pwd)

2clean:
	make clean
	rm -f main