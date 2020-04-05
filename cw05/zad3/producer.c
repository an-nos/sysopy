#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#define MAX_LINE_LEN 100

int main(int argc, char** argv) {
	char *pipe_path = argv[1];
	char *file_path = argv[2];
	int chars_per_write = atoi(argv[3]);

	srand(time(NULL));

	FILE* pipe = fopen(pipe_path, "w");

	FILE* file = fopen(file_path, "r");

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
		printf("%s", line);

		fwrite(line, sizeof(char), strlen(line), pipe);
	}

	fclose(pipe);
	fclose(file);

	exit(EXIT_SUCCESS);


}