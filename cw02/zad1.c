#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>

char** create_random(int line_count, int line_len){

	int n = 94;
	int start = 32;
	char chars[n];
	for(int i = 0; i<n; i++){
		chars[i] = (char) (start + i);
	}

	srand(time(0));
	char** strings = calloc(line_count, sizeof(char*));
	for(int i = 0; i<line_count; i++) {
		strings[i] = calloc(line_len + 1, sizeof(char));
		for (int j = 0; j<line_len; j++){
			strings[i][j] = chars[rand()%n];
		}
		strings[i][line_len] = '\0';
	}

	for(int i = 0; i<line_count; i++){
		printf("%s\n",strings[i]);
	}
	return strings;

}


void generate(char* file_name, int line_count, int line_len, int use_f){
	if(use_f == 1){
		FILE* file = fopen(file_name, "rw");

	}

}

int main(int argc, char** argv){
	create_random(5,4);
}