#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/times.h>
#include<errno.h>

extern int errno;

struct saved_times{
	double user;
	double system;
}; typedef struct saved_times saved_times;

void handle_error(char operation){
	switch (operation) {
		case 'r':
			fprintf(stderr, "Error reading line in file: %s\n", strerror(errno));
			break;
		case 'w':
			fprintf(stderr, "Error writing line in file: %s\n", strerror(errno));
			break;
		case 'o':
			fprintf(stderr, "Error opening file: %s\n", strerror(errno));
			break;
		default:
			fprintf(stderr, "Error: %s\n", strerror(errno));
			break;
	}
	exit(EXIT_FAILURE);
}

saved_times* save_times(struct tms* time){
	saved_times* new_times = malloc(sizeof(saved_times));
	new_times->user = time->tms_utime;
	new_times->system = time->tms_stime;
	return new_times;
}

saved_times* calc_times_diff(saved_times* start_times, saved_times* stop_times){
	saved_times* difference = malloc(sizeof(saved_times));
	difference->user = (stop_times->user - start_times->user)/sysconf(_SC_CLK_TCK);
	difference->system = (stop_times->system - start_times->system);//sysconf(_SC_CLK_TCK);
	return difference;
}

char** create_empty_strings(int line_count, int line_len){
	char** strings = calloc(line_count, sizeof(char*));
	for(int i = 0; i<line_count; i++){
		strings[i] = calloc(line_len+1, sizeof(char));
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
	return strings;
}

void generate(char* file_name, int line_count, int line_len, int use_f){
	char** strings = create_random(line_count, line_len);
	if(use_f == 1){
		FILE* file = fopen(file_name, "w+");
		if(file == NULL) handle_error('o');
		for(int i = 0; i < line_count; i++) {
			if(fwrite(strings[i], sizeof(char), line_len+1, file) != line_len+1) handle_error('w');
		}
		fclose(file);
	}
	else{
		int fd = open(file_name, O_WRONLY | O_CREAT, 0666);
		if(fd == -1) handle_error('o');

		for(int i = 0; i < line_count; i++){
			if(write(fd, strings[i], line_len+1) == -1) handle_error('w');
		}
		close(fd);
	}
	delete_strings(strings, line_count);
}

void copy(char* file_name, char* copy_name, int line_count, int line_len, int use_f){
	char ** strings = create_empty_strings(line_count, line_len);
	if(use_f == 1) {

		FILE *file = fopen(file_name, "r");
		FILE *copy = fopen(copy_name, "w+");
		if(file == NULL || copy == NULL) handle_error('o');

		for (int i = 0; i < line_count; i++) {
			if(fread(strings[i], sizeof(char), line_len+1, file) != line_len+1) handle_error('r');
			if(fwrite(strings[i], sizeof(char), line_len+1, copy) != line_len+1) handle_error('w');
		}
		fclose(file);
		fclose(copy);
	}
	else{
		int fd = open(file_name, O_RDONLY);

		int cd = open(copy_name, O_WRONLY | O_CREAT, 0666);
		for(int i = 0; i < line_count; i++){
			if(read(fd, strings[i], line_len+1) == -1) handle_error('r');
			if(write(cd, strings[i], line_len+1) == -1) handle_error('w');
		}
		close(fd);
		close(cd);
	}
	delete_strings(strings, line_count);
}

void seek_and_read_lib(char* line, int offset, int len, FILE* file){
	if(fseek(file, offset, 0) != 0 ||
		fread(line, sizeof(char), len, file) != len) handle_error('r');
}

void seek_and_write_lib(char* line, int offset, int len, FILE* file){
	if(fseek(file, offset, 0) != 0) handle_error('r');
	if(fwrite(line, sizeof(char), len, file) != len) handle_error('w');
}

void swap_lines_lib(FILE* file, int i, int j, int line_len){
	char* line1 = calloc(line_len+1, sizeof(char));
	char* line2 = calloc(line_len+1, sizeof(char));

	seek_and_read_lib(line1, (line_len+1)*i, line_len+1, file);
	seek_and_read_lib(line2, (line_len+1)*j, line_len+1, file);

	seek_and_write_lib(line1, (line_len+1)*j, line_len+1, file);
	seek_and_write_lib(line2, (line_len+1)*i, line_len+1, file);

	free(line1);
	free(line2);
}

int partition_lib(FILE* file, int l, int h, int line_len){
	char* pivot = calloc(line_len+1, sizeof(char));
	seek_and_read_lib(pivot, (line_len+1)*h, line_len+1, file);

	int i = l - 1;
	char* curr = calloc(line_len+1, sizeof(char));
	for(int j = l; j<h; j++){
		seek_and_read_lib(curr, (line_len+1)*j, line_len+1, file);
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

void seek_and_read_sys(char* line, int offset, int len, int fd){
	if(lseek(fd, offset, 0) == offset - 1 ||
	   read(fd, line, len) == -1) handle_error('r');
}

void seek_and_write_sys(char* line, int offset, int len, int fd){
	if(lseek(fd, offset, 0) == offset -1) handle_error('r');
	if(write(fd, line, len) == -1) handle_error('w');
}

void swap_lines_sys(int fd, int i, int j, int line_len){
	char* line1 = calloc(line_len+1, sizeof(char));
	char* line2 = calloc(line_len+1, sizeof(char));

	seek_and_read_sys(line1, (line_len+1)*i, line_len+1, fd);
	seek_and_read_sys(line2, (line_len+1)*j, line_len+1, fd);

	seek_and_write_sys(line1, (line_len+1)*j, line_len+1, fd);
	seek_and_write_sys(line2, (line_len+1)*i, line_len+1, fd);

	free(line1);
	free(line2);
}

int partition_sys(int fd, int l, int h, int line_len){
	char* pivot = calloc(line_len+1, sizeof(char));

	seek_and_read_sys(pivot, (line_len+1)*h, line_len+1, fd);

	int i = l -1;
	char* curr = calloc(line_len+1, sizeof(char));
	for(int j = l; j<h; j++){

		seek_and_read_sys(curr, (line_len+1)*j, line_len+1, fd);

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
		if(file == NULL) handle_error('o');
		quick_sort_lib(file, 0, line_count - 1, line_len);
		fclose(file);
	}
	else{
		int fd = open(file_name, O_RDWR);
		if(fd == -1) handle_error('o');
		quick_sort_sys(fd, 0, line_count - 1, line_len);
		close(fd);
	}
}


int main(int argc, char** argv){
	struct tms* start = malloc(sizeof(struct tms));
	struct tms* stop = malloc(sizeof(struct tms));
	times(start);
	saved_times* start_times = save_times(start);
	char* error_line = "Invalid number of arguments.";
	if(argc < 5){
		printf("%s\n", error_line);
		exit(EXIT_FAILURE);
	}

	int line_count, line_len, use_f = 1;
	char* command = argv[1];
	char* file_name = argv[2];
	char* mode;

	if(strcmp(command, "generate") == 0) {
		error_line = "Invalid arguments (expected: generate filename line_count line_length [sys/lib])";
		line_count = atoi(argv[3]);
		line_len = atoi(argv[4]);
		if (line_count == 0 || line_len == 0) {
			printf("%s\n", error_line);
			exit(EXIT_FAILURE);
		}

		if(argc == 6){
			mode = argv[5];
			if (strcmp(mode, "sys")) use_f = 0;
		}
		generate(file_name, line_count, line_len, use_f);
	}
	else if(strcmp(command, "sort") == 0){
		error_line = "Invalid arguments (expected: sort file_name line_count line_length sys/lib)";
		if(argc < 6){
			printf("%s\n", error_line);
			exit(EXIT_FAILURE);
		}
		line_count = atoi(argv[3]);
		line_len = atoi(argv[4]);
		mode = argv[5];
		if(line_count == 0 || line_len == 0 || (strcmp(mode, "sys") != 0 && strcmp(mode, "lib") != 0)){
			printf("%s\n", error_line);
			exit(EXIT_FAILURE);
		}

		if(strcmp(mode, "sys") == 0) use_f = 0;
		sort(file_name, line_count, line_len, use_f);
	}

	else if(strcmp(command, "copy") == 0){
		error_line = "Invalid arguments (expected: copy file_name copy_name line_count line_length sys/lib)";
		if(argc < 7){
			printf("%s\n",error_line);
			exit(EXIT_SUCCESS);
		}
		char* copy_name = argv[3];
		line_count = atoi(argv[4]);
		line_len = atoi(argv[5]);
		mode = argv[6];
		if(line_count == 0 || line_len == 0 || (strcmp(mode, "sys") != 0 && strcmp(mode, "lib") != 0)){
			printf("%s\n", error_line);
			exit(EXIT_FAILURE);
		}

		if(strcmp(mode,"sys") == 0) use_f = 0;
		copy(file_name, copy_name, line_count, line_len, use_f);
	}
	else{
		error_line = "Invalid command";
		printf("%s\n",error_line);
		exit(EXIT_FAILURE);
	}
	times(stop);
	saved_times* stop_times = save_times(stop);
	saved_times* difference = calc_times_diff(start_times, stop_times);
	printf("OPERATION: %s\n",command);
	printf("MODE: %s\n",mode);
	printf("LINES: %d\nLENGTH: %d\n", line_count, line_len);
	printf("%-10s %-10s\n","SYS","USER");
	printf("%-10f %-10f\n", difference->system, difference->user);
	exit(EXIT_SUCCESS);

}

