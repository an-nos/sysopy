#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#define __USE_XOPEN_EXTENDED 1
#include<ftw.h>
#include<limits.h>
#include <sys/wait.h>

char file_name[256];

char* concat_name(char* path, char* curr){
	char* new_path = calloc(strlen(path) + strlen(curr) + 2, sizeof(char));
	strcpy(new_path, path);
	strcat(new_path, "/");
	strcat(new_path, curr);
	return new_path;
}


void search_in_dir(char* dir_path, int depth){

	DIR* dir;
	dir = opendir(dir_path);
	if(dir == NULL){
		printf("Could not open directory");
		exit(EXIT_FAILURE);
	}

	struct stat st;
	lstat(dir_path, &st);
	if(S_ISDIR(st.st_mode)) {
		pid_t child_pid;
		child_pid = fork();
		if (child_pid < 0) {
			printf("Error forking.\n");
			exit(EXIT_FAILURE);
		} else if (child_pid == 0) {
			int curr_pid = getpid();
			printf("Path %s\n", dir_path);
			printf("PID: %d\n", curr_pid);
			execlp("ls", "ls", "-l", dir_path, NULL);
		} else {
			wait(0);
		}
	}
	struct dirent* curr;
	while((curr = readdir(dir))!= NULL){
		struct stat buf;
		char* new_path = concat_name(dir_path, curr->d_name);
		if(lstat(new_path, &buf) != 0) continue;
		if(!S_ISLNK(buf.st_mode) && S_ISDIR(buf.st_mode) && strcmp(".", curr->d_name) != 0 && strcmp("..", curr->d_name) != 0){
			search_in_dir(new_path, depth+1);
		}
		free(new_path);
	}
	closedir(dir);
}

static int nftw_step(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){
	if(S_ISDIR(sb->st_mode)) {
		pid_t child_pid;
		child_pid = fork();
		if (child_pid < 0) {
			printf("Error forking.\n");
			exit(EXIT_FAILURE);
		} else if (child_pid == 0) {
			int curr_pid = getpid();
			printf("Path %s\n", fpath);
			printf("PID: %d\n", curr_pid);
			execlp("ls", "ls", "-l", fpath, NULL);
		} else {
			wait(0);
		}
	}
	return 0;
}


int main(int argc, char** argv){
	char directory_name[100];
	int nftw_mode = 0;
	strcpy(directory_name, "");
	if(argc < 2){
		printf("Too few arguments.\n Expected: DIRECTORY_NAME [nftw/opendir]");
		exit(EXIT_FAILURE);
	}

	strcpy(directory_name, argv[1]);
	if(argc == 3){
		if(strcmp("nftw", argv[2]) == 0) nftw_mode = 1;
	}

	if(nftw_mode == 1){
		nftw(directory_name, nftw_step, 20, FTW_PHYS | FTW_DEPTH);
	}
	else {
		search_in_dir(directory_name, 1);
	}

}