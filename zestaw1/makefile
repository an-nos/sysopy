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


3static: main.c
	make static
	$(CC) -c main.c
	$(CC) main.o library.a -o main
	make clean

3shared: main.c
	make shared
	$(CC) main.c library.so -L ./ -o main -Wl,-rpath,$(shell pwd)
	make clean

3dynamic: main_dyn.c
	make shared
	$(CC) main_dyn.c -ldl -o main_dyn -I .

3clean:
	make clean
	rm -f main

3bstatic:
	make CC="gcc -O$(Oval)" 3static

3bshared:
	make CC="gcc -O$(Oval)" 3shared

3bdynamic:
	make CC="gcc -O$(Oval)" 3dynamic


test_static:
	make 3static
	@echo "test static" >> raport3a.txt
	make test >> raport3a.txt

test_shared:
	make 3shared
	@echo "test shared" >> raport3a.txt
	make test >> raport3a.txt

test_dynamic:
	make 3dynamic
	@echo "test dynamic" >> raport3a.txt
	make test_dyn >> raport3a.txt

test_staticO:
	make 3bstatic
	@echo "test static O$(Oval)" >> raport3b.txt
	make test >> raport3b.txt

test_sharedO:
	make 3bshared
	@echo "test shared O$Oval" >> raport3b.txt
	make test >> raport3b.txt

test_dynamicO:
	make 3bdynamic
	@echo "test dynamic O$Oval">> raport3b.txt
	make test_dyn >> raport3b.txt

test:
	@echo "5 different pairs"
	@./main 5 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt
	@echo "10 different pairs"
	@./main 10 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt ./files/different/file15-a.txt:./files/different/file15-b.txt ./files/different/file16-a.txt:./files/different/file16-b.txt ./files/different/file17-a.txt:./files/different/file17-b.txt ./files/different/file18-a.txt:./files/different/file18-b.txt ./files/different/file19-a.txt:./files/different/file19-b.txt
	@echo "25 different pairs"
	@./main 25 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt ./files/different/file15-a.txt:./files/different/file15-b.txt ./files/different/file16-a.txt:./files/different/file16-b.txt ./files/different/file17-a.txt:./files/different/file17-b.txt ./files/different/file18-a.txt:./files/different/file18-b.txt ./files/different/file19-a.txt:./files/different/file19-b.txt ./files/different/file1-a.txt:./files/different/file1-b.txt ./files/different/file20-a.txt:./files/different/file20-b.txt ./files/different/file21-a.txt:./files/different/file21-b.txt ./files/different/file22-a.txt:./files/different/file22-b.txt ./files/different/file23-a.txt:./files/different/file23-b.txt ./files/different/file24-a.txt:./files/different/file24-b.txt ./files/different/file25-a.txt:./files/different/file25-b.txt ./files/different/file2-a.txt:./files/different/file2-b.txt ./files/different/file3-a.txt:./files/different/file3-b.txt ./files/different/file4-a.txt:./files/different/file4-b.txt ./files/different/file5-a.txt:./files/different/file5-b.txt ./files/different/file6-a.txt:./files/different/file6-b.txt ./files/different/file7-a.txt:./files/different/file7-b.txt ./files/different/file8-a.txt:./files/different/file8-b.txt ./files/different/file9-a.txt:./files/different/file9-b.txt
	@echo "5 medium pairs"
	@./main 5 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt
	@echo "10 medium pairs"
	@./main 10 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt ./files/medium/file15-a.txt:./files/medium/file15-b.txt ./files/medium/file16-a.txt:./files/medium/file16-b.txt ./files/medium/file17-a.txt:./files/medium/file17-b.txt ./files/medium/file18-a.txt:./files/medium/file18-b.txt ./files/medium/file19-a.txt:./files/medium/file19-b.txt
	@echo "25 medium pairs"
	@./main 25 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt ./files/medium/file15-a.txt:./files/medium/file15-b.txt ./files/medium/file16-a.txt:./files/medium/file16-b.txt ./files/medium/file17-a.txt:./files/medium/file17-b.txt ./files/medium/file18-a.txt:./files/medium/file18-b.txt ./files/medium/file19-a.txt:./files/medium/file19-b.txt ./files/medium/file1-a.txt:./files/medium/file1-b.txt ./files/medium/file20-a.txt:./files/medium/file20-b.txt ./files/medium/file21-a.txt:./files/medium/file21-b.txt ./files/medium/file22-a.txt:./files/medium/file22-b.txt ./files/medium/file23-a.txt:./files/medium/file23-b.txt ./files/medium/file24-a.txt:./files/medium/file24-b.txt ./files/medium/file25-a.txt:./files/medium/file25-b.txt ./files/medium/file2-a.txt:./files/medium/file2-b.txt ./files/medium/file3-a.txt:./files/medium/file3-b.txt ./files/medium/file4-a.txt:./files/medium/file4-b.txt ./files/medium/file5-a.txt:./files/medium/file5-b.txt ./files/medium/file6-a.txt:./files/medium/file6-b.txt ./files/medium/file7-a.txt:./files/medium/file7-b.txt ./files/medium/file8-a.txt:./files/medium/file8-b.txt ./files/medium/file9-a.txt:./files/medium/file9-b.txt
	@echo "5 similar pairs"
	@./main 5 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt
	@echo "10 similar pairs"
	@./main 10 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt ./files/similar/file15-a.txt:./files/similar/file15-b.txt ./files/similar/file16-a.txt:./files/similar/file16-b.txt ./files/similar/file17-a.txt:./files/similar/file17-b.txt ./files/similar/file18-a.txt:./files/similar/file18-b.txt ./files/similar/file19-a.txt:./files/similar/file19-b.txt
	@echo "25 similar pairs"
	@./main 25 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt ./files/similar/file15-a.txt:./files/similar/file15-b.txt ./files/similar/file16-a.txt:./files/similar/file16-b.txt ./files/similar/file17-a.txt:./files/similar/file17-b.txt ./files/similar/file18-a.txt:./files/similar/file18-b.txt ./files/similar/file19-a.txt:./files/similar/file19-b.txt ./files/similar/file1-a.txt:./files/similar/file1-b.txt ./files/similar/file20-a.txt:./files/similar/file20-b.txt ./files/similar/file21-a.txt:./files/similar/file21-b.txt ./files/similar/file22-a.txt:./files/similar/file22-b.txt ./files/similar/file23-a.txt:./files/similar/file23-b.txt ./files/similar/file24-a.txt:./files/similar/file24-b.txt ./files/similar/file25-a.txt:./files/similar/file25-b.txt ./files/similar/file26-a.txt:./files/similar/file26-b.txt ./files/similar/file27-a.txt:./files/similar/file27-b.txt ./files/similar/file28-a.txt:./files/similar/file28-b.txt ./files/similar/file29-a.txt:./files/similar/file29-b.txt ./files/similar/file2-a.txt:./files/similar/file2-b.txt ./files/similar/file30-a.txt:./files/similar/file30-b.txt ./files/similar/file31-a.txt:./files/similar/file31-b.txt ./files/similar/file32-a.txt:./files/similar/file32-b.txt
	@echo "compare 5 different 5 medium 5 similar"
	@./main 15 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt
	@echo "compare 25 different, remove blocks 0 0 0 0 2"
	@./main 10 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt ./files/similar/file15-a.txt:./files/similar/file15-b.txt ./files/similar/file16-a.txt:./files/similar/file16-b.txt ./files/similar/file17-a.txt:./files/similar/file17-b.txt ./files/similar/file18-a.txt:./files/similar/file18-b.txt ./files/similar/file19-a.txt:./files/similar/file19-b.txt remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 2
	@echo "compare 5 similar 5 different remove blocks 0 7 0 5"
	@./main 10 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt 	./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt remove_block 0 remove_block 7 remove_block 0 remove_block 4
	@echo "compare 5 medium remove block 0 compare 5 different remove block 0 compare 5 similar remove block 7"
	@./main 15 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt remove_block 0 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt remove_block 0 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt remove_block 7

test_dyn:
	@echo "5 different pairs"
	@./main_dyn 5 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt
	@echo "10 different pairs"
	@./main_dyn 10 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt ./files/different/file15-a.txt:./files/different/file15-b.txt ./files/different/file16-a.txt:./files/different/file16-b.txt ./files/different/file17-a.txt:./files/different/file17-b.txt ./files/different/file18-a.txt:./files/different/file18-b.txt ./files/different/file19-a.txt:./files/different/file19-b.txt
	@echo "25 different pairs"
	@./main_dyn 25 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt ./files/different/file15-a.txt:./files/different/file15-b.txt ./files/different/file16-a.txt:./files/different/file16-b.txt ./files/different/file17-a.txt:./files/different/file17-b.txt ./files/different/file18-a.txt:./files/different/file18-b.txt ./files/different/file19-a.txt:./files/different/file19-b.txt ./files/different/file1-a.txt:./files/different/file1-b.txt ./files/different/file20-a.txt:./files/different/file20-b.txt ./files/different/file21-a.txt:./files/different/file21-b.txt ./files/different/file22-a.txt:./files/different/file22-b.txt ./files/different/file23-a.txt:./files/different/file23-b.txt ./files/different/file24-a.txt:./files/different/file24-b.txt ./files/different/file25-a.txt:./files/different/file25-b.txt ./files/different/file2-a.txt:./files/different/file2-b.txt ./files/different/file3-a.txt:./files/different/file3-b.txt ./files/different/file4-a.txt:./files/different/file4-b.txt ./files/different/file5-a.txt:./files/different/file5-b.txt ./files/different/file6-a.txt:./files/different/file6-b.txt ./files/different/file7-a.txt:./files/different/file7-b.txt ./files/different/file8-a.txt:./files/different/file8-b.txt ./files/different/file9-a.txt:./files/different/file9-b.txt
	@echo "5 medium pairs"
	@./main_dyn 5 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt
	@echo "10 medium pairs"
	@./main_dyn 10 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt ./files/medium/file15-a.txt:./files/medium/file15-b.txt ./files/medium/file16-a.txt:./files/medium/file16-b.txt ./files/medium/file17-a.txt:./files/medium/file17-b.txt ./files/medium/file18-a.txt:./files/medium/file18-b.txt ./files/medium/file19-a.txt:./files/medium/file19-b.txt
	@echo "25 medium pairs"
	@./main_dyn 25 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt ./files/medium/file15-a.txt:./files/medium/file15-b.txt ./files/medium/file16-a.txt:./files/medium/file16-b.txt ./files/medium/file17-a.txt:./files/medium/file17-b.txt ./files/medium/file18-a.txt:./files/medium/file18-b.txt ./files/medium/file19-a.txt:./files/medium/file19-b.txt ./files/medium/file1-a.txt:./files/medium/file1-b.txt ./files/medium/file20-a.txt:./files/medium/file20-b.txt ./files/medium/file21-a.txt:./files/medium/file21-b.txt ./files/medium/file22-a.txt:./files/medium/file22-b.txt ./files/medium/file23-a.txt:./files/medium/file23-b.txt ./files/medium/file24-a.txt:./files/medium/file24-b.txt ./files/medium/file25-a.txt:./files/medium/file25-b.txt ./files/medium/file2-a.txt:./files/medium/file2-b.txt ./files/medium/file3-a.txt:./files/medium/file3-b.txt ./files/medium/file4-a.txt:./files/medium/file4-b.txt ./files/medium/file5-a.txt:./files/medium/file5-b.txt ./files/medium/file6-a.txt:./files/medium/file6-b.txt ./files/medium/file7-a.txt:./files/medium/file7-b.txt ./files/medium/file8-a.txt:./files/medium/file8-b.txt ./files/medium/file9-a.txt:./files/medium/file9-b.txt
	@echo "5 similar pairs"
	@./main_dyn 5 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt
	@echo "10 similar pairs"
	@./main_dyn 10 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt ./files/similar/file15-a.txt:./files/similar/file15-b.txt ./files/similar/file16-a.txt:./files/similar/file16-b.txt ./files/similar/file17-a.txt:./files/similar/file17-b.txt ./files/similar/file18-a.txt:./files/similar/file18-b.txt ./files/similar/file19-a.txt:./files/similar/file19-b.txt
	@echo "25 similar pairs"
	@./main_dyn 25 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt ./files/similar/file15-a.txt:./files/similar/file15-b.txt ./files/similar/file16-a.txt:./files/similar/file16-b.txt ./files/similar/file17-a.txt:./files/similar/file17-b.txt ./files/similar/file18-a.txt:./files/similar/file18-b.txt ./files/similar/file19-a.txt:./files/similar/file19-b.txt ./files/similar/file1-a.txt:./files/similar/file1-b.txt ./files/similar/file20-a.txt:./files/similar/file20-b.txt ./files/similar/file21-a.txt:./files/similar/file21-b.txt ./files/similar/file22-a.txt:./files/similar/file22-b.txt ./files/similar/file23-a.txt:./files/similar/file23-b.txt ./files/similar/file24-a.txt:./files/similar/file24-b.txt ./files/similar/file25-a.txt:./files/similar/file25-b.txt ./files/similar/file26-a.txt:./files/similar/file26-b.txt ./files/similar/file27-a.txt:./files/similar/file27-b.txt ./files/similar/file28-a.txt:./files/similar/file28-b.txt ./files/similar/file29-a.txt:./files/similar/file29-b.txt ./files/similar/file2-a.txt:./files/similar/file2-b.txt ./files/similar/file30-a.txt:./files/similar/file30-b.txt ./files/similar/file31-a.txt:./files/similar/file31-b.txt ./files/similar/file32-a.txt:./files/similar/file32-b.txt
	@echo "compare 5 different 5 medium 5 similar"
	@./main_dyn 15 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt
	@echo "compare 25 different, remove blocks 0 0 0 0 2"
	@./main_dyn 10 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt ./files/similar/file15-a.txt:./files/similar/file15-b.txt ./files/similar/file16-a.txt:./files/similar/file16-b.txt ./files/similar/file17-a.txt:./files/similar/file17-b.txt ./files/similar/file18-a.txt:./files/similar/file18-b.txt ./files/similar/file19-a.txt:./files/similar/file19-b.txt remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 2
	@echo "compare 5 similar 5 different remove blocks 0 7 0 5"
	@./main_dyn 10 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt 	./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt remove_block 0 remove_block 7 remove_block 0 remove_block 4
	@echo "compare 5 medium remove block 0 compare 5 different remove block 0 compare 5 similar remove block 7"
	@./main_dyn 15 compare_pairs ./files/medium/file10-a.txt:./files/medium/file10-b.txt ./files/medium/file11-a.txt:./files/medium/file11-b.txt ./files/medium/file12-a.txt:./files/medium/file12-b.txt ./files/medium/file13-a.txt:./files/medium/file13-b.txt ./files/medium/file14-a.txt:./files/medium/file14-b.txt remove_block 0 compare_pairs ./files/different/file10-a.txt:./files/different/file10-b.txt ./files/different/file11-a.txt:./files/different/file11-b.txt ./files/different/file12-a.txt:./files/different/file12-b.txt ./files/different/file13-a.txt:./files/different/file13-b.txt ./files/different/file14-a.txt:./files/different/file14-b.txt remove_block 0 compare_pairs ./files/similar/file10-a.txt:./files/similar/file10-b.txt ./files/similar/file11-a.txt:./files/similar/file11-b.txt ./files/similar/file12-a.txt:./files/similar/file12-b.txt ./files/similar/file13-a.txt:./files/similar/file13-b.txt ./files/similar/file14-a.txt:./files/similar/file14-b.txt remove_block 7


