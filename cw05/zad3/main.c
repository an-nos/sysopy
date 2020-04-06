#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#define MAX_COMMAND_LEN 100

void generate_file(char* file_name, int i){
	FILE* f = fopen(file_name, "w+");
	int lines = random()%5+5;
	char character = 'a' + i;
	int chars_per_line = random()%4+2;
	for(int j = 0; j < lines; j++){
		for(int k = 0; k < chars_per_line; k++){
			fwrite(&character, sizeof(char), 1, f);
		}
		fwrite("\n", sizeof(char), 1, f);

	}
	fclose(f);
}

int main(int argc, char** argv){
	char* fifo_name = "pipe";
	mkfifo(fifo_name, 0666);

	pid_t pids[6];

	srand(time(NULL));


	char file_name[MAX_COMMAND_LEN];
	char num_str[2];

	char* numbers_of_chars[5] = {"5", "3", "4", "6", "4"};
	for(int i = 1; i <= 5; i++){

		strcpy(file_name, "prod");
		sprintf(num_str, "%d", i);
		strcat(file_name, num_str);

		generate_file(file_name, i);

		pids[i-1] = fork();
		if(pids[i-1] == 0){
			execl("./producer", "./producer", fifo_name, file_name, numbers_of_chars[i-1], NULL);
		}
		printf("producer executed\n");

	}


	pids[5] = fork();
	if(pids[5] == 0){
		execl("./consumer", "./consumer", fifo_name, "cons1", "5", NULL);
	}
	printf("consumer executed\n");


	for(int i = 0; i<7; i++){
		int status;
		waitpid(pids[i], &status, 0);
	}

	exit(EXIT_SUCCESS);
}