CC = gcc
FLAGS = -Wall
LINES = 4000
LEN = 5000

all: main

main: zad2.c
	$(CC) $(FLAGS) -o zad2 zad2.c

clean:
	rm -f zad2

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
	./zad2 -name find_me -maxdepth 2
	make clean_all

clean_all:
	make clean
	rm -f find_me
	rm -f link1
	rm -f link2
	rm -r dir1


