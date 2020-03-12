#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

char** create_empty_strings(int line_count, int line_len){
	char** strings = calloc(line_count, sizeof(char*));
	for(int i = 0; i<line_count; i++){
		strings[i] = calloc(line_len+1, sizeof(char));
		strcat(strings[i],"");
	}
	return strings;
}

void delete_strings(char** strings, int line_count){
	for(int i = 0; i<line_count; i++){
		free(strings[i]);
	}
	free(strings);
}
char** create_random(int line_count, int line_len){
	int n = 26;
	int start = 97;
	char chars[n];
	for(int i = 0; i<n; i++){
		chars[i] = (char) (start + i);
	}

	srand(time(0));
	char** strings = calloc(line_count, sizeof(char*));
	for(int i = 0; i<line_count; i++) {
		strings[i] = calloc(line_len + 1, sizeof(char));
		for (int j = 0; j<line_len; j++){
			strings[i][j] = chars[rand()%n];
		}
		strings[i][line_len] = '\n';
	}

//	for(int i = 0; i<line_count; i++) printf("%s\n",strings[i]);
	return strings;
}

void generate(char* file_name, int line_count, int line_len, int use_f){
	char** strings = create_random(line_count, line_len);
	if(use_f == 1){
		FILE* file = fopen(file_name, "w+");
		for(int i = 0; i < line_count; i++) {
			fwrite(strings[i], sizeof(char), line_len+1, file);
		}
		fclose(file);
	}
	else{
		int fd = open(file_name, O_WRONLY | O_CREAT);
		for(int i = 0; i < line_count; i++){
			write(fd, strings[i], line_len+1);
		}
		close(fd);
	}
	delete_strings(strings, line_count);
}

void copy(char* file_name, char* copy_name, int line_count, int line_len, int use_f){
	char ** strings = create_empty_strings(line_count, line_len);
	if(use_f == 1) {
		FILE *file = fopen(file_name, "r");
		FILE *copy = fopen(copy_name, "w");
		for (int i = 0; i < line_count; i++) {
			fread(strings[i], sizeof(char), line_len + 1, file);
			fwrite(strings[i], sizeof(char), line_len + 1, copy);
		}
		fclose(file);
		fclose(copy);
	}
	else{
		int fd = open(file_name, O_RDONLY);
		int cd = open(copy_name, O_WRONLY | O_CREAT);
		for(int i =0; i < line_count; i++){
			read(fd, strings[i], line_len+1);
			write(cd, strings[i], line_len+1);
		}
		close(fd);
		close(cd);
	}
	delete_strings(strings, line_count);
}

void swap_lines_lib(FILE* file, int i, int j, int line_len){
	char* line1 = calloc(line_len+1, sizeof(char));
	char* line2 = calloc(line_len+1, sizeof(char));

	fseek(file, (line_len+1)*i, 0);
	fread(line1, sizeof(char),line_len+1, file);

	fseek(file, (line_len+1)*j, 0);
	fread(line2, sizeof(char), line_len+1, file);

	fseek(file, (line_len+1)*j, 0);
	fwrite(line1, sizeof(char), line_len+1 ,file);
	fseek(file, (line_len+1)*i, 0);
	fwrite(line2, sizeof(char), line_len+1, file);

	free(line1);
	free(line2);
}

int partition_lib(FILE* file, int l, int h, int line_len){
	char* pivot = calloc(line_len+1, sizeof(char));
	fseek(file, (line_len+1)*h, 0);
	fread(pivot, sizeof(char), line_len+1, file);
	int i = l - 1;
	char* curr = calloc(line_len+1, sizeof(char));
	for(int j = l; j<h; j++){
		fseek(file, (line_len+1)*j, 0);
		fread(curr, sizeof(char), line_len+1, file);
		if(strcmp(curr,pivot)<0){
			swap_lines_lib(file,++i,j,line_len);
		}
	}
	free(curr);
	free(pivot);
	swap_lines_lib(file, ++i, h, line_len);
	return i;
}

void quick_sort_lib(FILE* file, int l, int h, int line_len){
	if(l<h){
		int p = partition_lib(file, l, h, line_len);
		quick_sort_lib(file, l, p-1, line_len);
		quick_sort_lib(file, p+1, h, line_len);
	}
}

void swap_lines_sys(int fd, int i, int j, int line_len){
	char* line1 = calloc(line_len+1, sizeof(char));
	char* line2 = calloc(line_len+1, sizeof(char));

	lseek(fd, (line_len+1)*i, 0);
	read(fd, line1, line_len+1);

	lseek(fd, (line_len+1)*j, 0);
	read(fd, line2, line_len+1);

	lseek(fd, (line_len+1)*j, 0);
	write(fd, line1, line_len+1);
	lseek(fd, (line_len+1)*i, 0);
	write(fd, line2, line_len+1);

	free(line1);
	free(line2);
}

int partition_sys(int fd, int l, int h, int line_len){
	char* pivot = calloc(line_len+1, sizeof(char));
	lseek(fd, (line_len+1)*h, 0);
	read(fd, pivot, line_len+1);
	int i = l -1;
	char* curr = calloc(line_len+1, sizeof(char));
	for(int j = l; j<h; j++){
		lseek(fd, (line_len+1)*j, 0);
		read(fd, curr, line_len+1);
		if(strcmp(curr, pivot)<0){
			swap_lines_sys(fd, ++i, j, line_len);
		}
	}
	free(curr);
	free(pivot);
	swap_lines_sys(fd, ++i, h, line_len);
	return i;
}

void quick_sort_sys(int fd, int l, int h, int line_len){
	if(l<h){
		int p = partition_sys(fd, l, h, line_len);
		quick_sort_sys(fd, l, p-1, line_len);
		quick_sort_sys(fd, p+1, h, line_len);
	}
}

void sort(char* file_name, int line_count, int line_len, int use_f){
	if(use_f == 1){
		FILE* file = fopen(file_name, "r+");
		quick_sort_lib(file, 0, line_count - 1, line_len);
		fclose(file);
	}
	else{
		int fd = open(file_name, O_RDWR);
		quick_sort_sys(fd, 0, line_count - 1, line_len);
		close(fd);
	}
}

int main(int argc, char** argv){
//	generate("dupa.txt", 5, 5, 1);
//	copy("dupa.txt","chuj.txt",5,5,0);
//	FILE* file = fopen("dupa.txt", "r+");
//	swap_lines_lib(file,1,2,5);
//	fclose(file);
//	int fd = open("dupa.txt", O_RDWR);
//	swap_lines_sys(fd, 1, 2, 5);
//	close(fd);
	sort("dupa.txt", 5, 5, 0);
}

