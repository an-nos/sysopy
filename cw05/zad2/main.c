#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN 100

int main(int argc, char** argv){
	if(argc<2){
		printf("Invalid arguments. Expected: file_name\n");
		exit(EXIT_FAILURE);
	}
	char* file_name = argv[1];

	char sort_command[FILENAME_MAX];
	char line[MAX_LINE_LEN];

	strcpy(sort_command, "sort ");
	strcat(sort_command, file_name);

	FILE* fp = popen(sort_command, "w");
	if(fp == NULL){
		printf("popen failed\n");
		exit(EXIT_FAILURE);
	}

	while(fgets(line, sizeof line, fp) != NULL){
		printf("%s", line);
	}

	pclose(fp);

}