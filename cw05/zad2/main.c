#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 100

int main(int argc, char** argv){

	char* file_name = argv[1];

	char sort_command[FILENAME_MAX];
	char line[MAX_LINE_LEN];

	strcpy(sort_command, "sort ");
	strcat(sort_command, file_name);

	FILE* fp = popen(sort_command, "w");

	while(fgets(line, sizeof line, fp) != NULL){
		printf("%s", line);
	}

	pclose(fp);

}