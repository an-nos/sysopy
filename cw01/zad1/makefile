CC = gcc
FLAGS = -Wall

all: static shared clean

static: library.c
	$(CC) $(FLAGS) -c library.c
	ar rcs library.a library.o
	make clean

shared: library.c
	$(CC) $(FLAGS) -fPIC -c library.c
	$(CC) $(FLAGS) -fPIC -shared -o library.so library.o
	make clean

clean:
	rm -f *.o

clean_all:
	make clean
	rm -f *.a
	rm -f *.so
	rm -f library