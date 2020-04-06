#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 100

int main(int argc, char** argv){
	if(argc<4){
		printf("Invalid arguments. Expected: pipe_name file_name characters_per_read\n");
		exit(EXIT_FAILURE);
	}
	char* pipe_path = argv[1];
	char* file_path = argv[2];
	int chars_per_read = atoi(argv[3]);

	FILE* pipe = fopen(pipe_path, "r");

	FILE* file = fopen(file_path, "w+");

	if(pipe == NULL || file == NULL){
		printf("fopen failed\n");
		exit(EXIT_FAILURE);
	}

	char buffer[MAX_LINE_LEN];

	while(fread(buffer, sizeof(char), chars_per_read, pipe) > 0){
		fwrite(buffer, sizeof(char), chars_per_read, file);
	}

	fclose(pipe);
	fclose(file);

	exit(EXIT_SUCCESS);
}