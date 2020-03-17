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

int max_depth = INT_MAX;
char file_name[256];
int atime_diff, mtime_diff;
char asign, msign;

void print_time(struct timespec time){
	char time_str[100];
	struct tm *times = localtime(&time);
	strftime(time_str, 100*sizeof(char), "%c", times);
	printf("%s\n",time_str);
}

int correct_time(struct timespec time, int time_diff, char sign){
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	int diff = (spec.tv_sec - time.tv_sec)/86400;
	if((sign == '+' && diff>time_diff) ||
		(sign == '-' && diff<time_diff) ||
		(sign == '=' && diff==time_diff))
		return 1;
	return 0;
}

int check_times(const struct stat* stat){
	if(atime_diff != 0 && correct_time(stat->st_atim, atime_diff, asign) == 0) return 0;
	if(mtime_diff != 0 && correct_time(stat->st_mtim, mtime_diff, msign) == 0) return 0;
	return 1;
}

void print_results(char* file_path){
	struct stat file_stat;
	if(stat(file_path, &file_stat) != 0) return;
	printf("PATH: ");
	if(file_path[0] != '/' && file_path[0] != '~'){
		char path[PATH_MAX];
		if(getcwd(path, sizeof(path)) != NULL){
			strcat(path, file_path+1);
			printf("%s\n", path);
		}
	}
	else printf("%s\n", file_path);
	printf("HARD LINKS: %ld\n",file_stat.st_nlink);
	printf("TYPE: ");
	switch (file_stat.st_mode & S_IFMT) {
		case S_IFREG:  printf("file\n");
		break;
		case S_IFDIR:  printf("dir\n");
		break;
		case S_IFCHR:  printf("char dev\n");
		break;
		case S_IFBLK:  printf("block dev\n");
		break;
		case S_IFIFO:  printf("fifo\n");
		break;
		case S_IFLNK:  printf("slink\n");
		break;
		case S_IFSOCK: printf("sock\n");
		break;
		default:	printf("other\n");
		break;
	}
	printf("SIZE: %ld\n", file_stat.st_size);

	printf("LAST ACCESS: ");
	print_time(file_stat.st_atim);
	printf("LAST MODIFIED: ");
	print_time(file_stat.st_mtim);
	printf("\n");

}

char* concat_name(char* path, char* curr){
	char* new_path = calloc(strlen(path) + strlen(curr) + 2, sizeof(char));
	strcpy(new_path, path);
	strcat(new_path, "/");
	strcat(new_path, curr);
	return new_path;
}
void search_in_dir(char* dir_path, int depth){
	if(depth > max_depth) return;

	DIR* dir;
	dir = opendir(dir_path);
	if(dir == NULL){
		printf("Could not open directory");
		exit(EXIT_FAILURE);
	}
	struct dirent* curr;
	while((curr = readdir(dir))!= NULL){
		struct stat buf;
		char* new_path = concat_name(dir_path, curr->d_name);
		if(lstat(new_path, &buf) != 0) continue;
		if(strcmp(file_name, curr->d_name) == 0 && check_times(&buf) == 1){
				print_results(new_path);
		}
		if(!S_ISLNK(buf.st_mode) && S_ISDIR(buf.st_mode) && strcmp(".", curr->d_name) != 0 && strcmp("..", curr->d_name) != 0){
			search_in_dir(new_path, depth+1);
		}
		free(new_path);
	}
	closedir(dir);
}

static int nftw_step(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){
	if(ftwbuf->level > max_depth) return 1;
	if(strcmp(fpath  + (strlen(fpath) - strlen(file_name)), file_name) == 0 && check_times(sb)){
		print_results((char*)fpath);
	}
	return 0;
}


int main(int argc, char** argv){
	char directory_name[100];
	mtime_diff = atime_diff = 0;
	int nftw_mode = 0;
	strcpy(directory_name, "");
	if(argc < 3){
		printf("Too few arguments.\n Expected: [DIRECTORY_NAME] -name FILE_NAME [-mtime MTIME] [-atime ATIME] [-maxdepth DEPTH] [nftw/opendir]\n default: opendir");
		exit(EXIT_FAILURE);
	}

	int shift = 0;
	if(strcmp(argv[1],"-name") != 0 ){
		strcpy(directory_name, argv[1]);
		shift = 1;
	}
	else {
		strcpy(directory_name, ".");
		if (strcmp(argv[1 + shift], "-name") != 0) {
			printf("Missing -name (FILE_NAME must be preceded with -name)");
			exit(EXIT_FAILURE);
		}
	}
	strcpy(file_name, argv[2 + shift]);
	int i = 3 + shift;
	while(i<argc){
		if(strcmp(argv[i], "-mtime") == 0){
			if(mtime_diff != 0){
				printf("Too many declarations of mtime.");
				exit(EXIT_FAILURE);
			}
			i++;
			if(argv[i][0] == '+' || argv[i][0] == '-') msign = argv[i][0];
			else msign = '=';
			mtime_diff = abs(atoi(argv[i]));
		}
		else if(strcmp(argv[i], "-atime") == 0){
			if(atime_diff != 0){
				printf("Too many declarations of atime.");
				exit(EXIT_FAILURE);
			}
			i++;
			if(argv[i][0] == '+' || argv[i][0] == '-') asign = argv[i][0];
			else asign = '=';
			atime_diff = abs(atoi(argv[i]));
		}
		else if(strcmp(argv[i], "-maxdepth") == 0){
			max_depth = atoi(argv[++i]);
			if(max_depth == 0){
				printf("Invalid argument of max_depth. Must be at least 1.");
				exit(EXIT_FAILURE);
			}
		}
		else if(strcmp(argv[i], "nftw") == 0) nftw_mode = 1;
		i++;
	}


	if(nftw_mode == 1){
		nftw(directory_name, nftw_step, 20, FTW_PHYS | FTW_DEPTH);
	}
	else {
		search_in_dir(directory_name, 1);
	}

}