#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_LINE_LEN 100

int main(int argc, char** argv) {
	if(argc<4){
		printf("Invalid arguments. Expected: pipe_name file_name characters_per_write\n");
		exit(EXIT_FAILURE);
	}
	char *pipe_path = argv[1];
	char *file_path = argv[2];
	int chars_per_write = atoi(argv[3]);

	srand(time(NULL));

	FILE* pipe = fopen(pipe_path, "w");

	FILE* file = fopen(file_path, "r");

	if(pipe == NULL || file == NULL){
		printf("fopen failed\n");
		exit(EXIT_FAILURE);
	}

	char buffer[MAX_LINE_LEN];
	char line[MAX_LINE_LEN];
	char pid_str[10];

	int pid = getpid();
	sprintf(pid_str, "#%d#", pid);

	while(fread(buffer, sizeof(char), chars_per_write, file) > 0){
		int seconds = 1 + rand()%2;
		sleep(seconds);
		strcpy(line, pid_str);
		strcat(line, buffer);
		int len = strlen(buffer) + chars_per_write;

		fwrite(line, sizeof(char), len, pipe);
	}

	fclose(pipe);
	fclose(file);

	exit(EXIT_SUCCESS);


}