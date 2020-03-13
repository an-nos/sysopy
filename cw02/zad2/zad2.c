#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void print_results(char* file_path){
	struct stat file_stat;
	if(stat(file_path, &file_stat) != 0) return;

	printf("found!\n");
}

char* concat_name(char* path, char* curr){
	char* new_path = calloc(strlen(path) + strlen(curr) + 1, sizeof(char));
	strcpy(new_path, path);
	strcat(new_path, "/");
	strcat(new_path, curr);
	return new_path;
}
void search_in_dir(char* file_name, char* dir_path){
	DIR* dir;
	dir = opendir(dir_path);
	if( dir == NULL){
		printf("Could not open directory");
		exit(EXIT_FAILURE);
	}
	struct dirent* curr;

	while((curr = readdir(dir))!= NULL){
		printf("%s\n",curr->d_name);
		char * new_path = concat_name(dir_path, curr->d_name);

		if(strcmp(file_name, curr->d_name) == 0){
			print_results(new_path);
		}
		struct stat buf;
		if(lstat(new_path, &buf) == 0 && !S_ISLNK(buf.st_mode) && S_ISDIR(buf.st_mode) && strcmp(".", curr->d_name) != 0 && strcmp("..", curr->d_name) != 0){
			printf("Recursive call on %s\n", new_path);
			search_in_dir(file_name, new_path);
		}

		free(new_path);
	}
	closedir(dir);

}


int main(int argc, char** argv){

	search_in_dir("find_me",".");
}