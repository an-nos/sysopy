CC = gcc

all: static shared clean

static: library.c
	$(CC) -c library.c
	ar rcs library.a library.o
	make clean

shared: library.c
	$(CC) -fPIC -c library.c
	$(CC) -fPIC -shared -o library.so library.o
	make clean

clean:
	rm -f *.o

clean_all:
	make clean
	rm -f *.a
	rm -f *.so
	rm -f library