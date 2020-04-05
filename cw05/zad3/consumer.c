#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LINE_LEN 100

int main(int argc, char** argv){
	char* pipe_path = argv[1];
	char* file_path = argv[2];
	int chars_per_read = atoi(argv[3]);

	FILE* pipe = fopen(pipe_path, "r");

	FILE* file = fopen(file_path, "w+");

	char buffer[MAX_LINE_LEN];

	while(fread(buffer, sizeof(char), chars_per_read, pipe) > 0){
		printf("%s", buffer);
		fwrite(buffer, sizeof(char), chars_per_read, file);
	}

	fclose(pipe);
	fclose(file);

	exit(EXIT_SUCCESS);
}