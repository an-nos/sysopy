CC = gcc
FLAGS = -Wall

all: main

main: main.c
	$(CC) $(FLAGS) -o main main.c

clean:
	rm -f main

test:
	touch find_me
	mkdir dir1
	mkdir dir1/dir2
	mkdir dir1/dir2/dir3
	touch dir1/find_me
	touch dir1/dir2/find_me
	touch dir1/dir2/dir3 find_me
	ln -s dir1 link1
	ln -s dir1/dir2 link2
	make main
	./main .
	make clean_all

clean_all:
	make clean
	rm -f find_me
	rm -f link1
	rm -f link2
	rm -r dir1


