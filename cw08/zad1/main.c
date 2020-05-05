#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#define GRAY_SCALE_MAX 256

enum mode{
	SIGN = 0,
	BLOCK = 1,
	INTERLEAVED = 2
}; typedef enum mode mode;

int** histogram;
int** image;
int width, height;
int threads_num;
mode g_mode;

void error_exit(char* message){
	printf("%s Error: %s\n", message, strerror(errno));
	exit(EXIT_FAILURE);
}

void initialize_histogram(){

	histogram = calloc(threads_num, sizeof(int*));
	for(int i = 0; i < threads_num; i++){
		histogram[i] = calloc(GRAY_SCALE_MAX, sizeof(int));
	}

	for(int i = 0; i < threads_num; i++) {
		for (int j = 0; j < GRAY_SCALE_MAX; j++) {
			histogram[i][j] = 0;
		}
	}
}

void create_image_array(){
	image = calloc(height, sizeof(int*));
	for(int i = 0; i < height; i++){
		image[i] = calloc(width, sizeof(int));
	}
}

void free_arrays(){
	for(int i = 0; i < height; i++){
		free(image[i]);
	}
	free(image);

	for(int i = 0; i < threads_num; i++){
		free(histogram[i]);
	}
	free(histogram);
}


void read_file(char* file_name){

	FILE* f = fopen(file_name, "r");
	if(f == NULL) error_exit("Could not open file.");

	char* line = NULL;
	size_t len = 0;
	char* buff;

	int line_idx = 0;
	int num_idx = 0;
	int num;

	while (getline(&line, &len, f) != -1) {
		if(line_idx == 1){

			buff = strtok(line, " ");
			width = atoi(buff);
			buff = strtok(NULL, "\n");
			height = atoi(buff);
			create_image_array();
		}
		else if(line_idx > 2){

			buff = strtok(line, " \n\t");

			while(buff != NULL){
				num = atoi(buff);
				image[num_idx / width][num_idx % width] = num;
				num_idx++;
				buff = strtok(NULL, " \n\t");

			}
		}
		line_idx++;
	}
	printf("\n");

		fclose(f);
	if(line) free(line);

}

long time_diff(struct timeval start, struct timeval stop){
	return (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
}

void* create_histogram(void* arg){

	struct timeval stop, start;

	int k = *((int*) arg);
	int x_start;
	int x_stop;

	gettimeofday(&start, NULL);

	switch(g_mode){
		case SIGN:
			for(int r = 0; r < height; r++){
				for(int c = 0; c < width; c++){
					if(image[r][c] % threads_num == k) histogram[k][image[r][c]]++;
				}
			}

			break;
		case BLOCK:
			x_start = k * ceil((double) width / threads_num);
			x_stop = (k + 1) * ceil ( (double) width / threads_num) - 1;
			for(int r = 0; r < height; r++) {
				for (int c = x_start; c <= x_stop; c++) {
					histogram[k][image[r][c]]++;
				}
			}
			break;
		case INTERLEAVED:
			for(int r = 0; r < height; r++){
				for(int c = k; c < width; c += threads_num){
					histogram[k][image[r][c]]++;
				}
			}
			break;
	}

	gettimeofday(&stop, NULL);

	long* delta_us = malloc(sizeof(long));
	*(delta_us) = time_diff(start, stop);

	pthread_exit(delta_us);
}

void save_to_file(char* output_file){
	FILE* f = fopen(output_file, "w+");
	for(int i = 0; i < GRAY_SCALE_MAX; i++){
		fprintf(f, "%d: %d\n", i, histogram[0][i]);
	}
}


int main(int argc, char** argv){

	if(argc < 5) error_exit("Too few arguments. Expected: threads_num sign/block/interleaved input_file output_file");

	threads_num = atoi(argv[1]);
	if(threads_num == 0) error_exit("Invalid threads number.");

	if(strcmp(argv[2], "sign") == 0) g_mode = SIGN;
	else if(strcmp(argv[2], "block") == 0) g_mode = BLOCK;
	else if(strcmp(argv[2], "interleaved") == 0) g_mode = INTERLEAVED;
	else error_exit("Invalid mode. Possible modes: sign/block/interleaved");


	printf("MODE: %s\t THREADS: %s\t FILE: %s\n", argv[2], argv[1], argv[3]);
	char* input_file = argv[3];
	char* output_file = argv[4];

	read_file(input_file);


	initialize_histogram();


	struct timeval stop, start;

	gettimeofday(&start, NULL);

	pthread_t* threads = calloc(threads_num, sizeof(pthread_t));

	for(int i = 0; i < threads_num; i++){
		int* k = malloc(sizeof(int));
		*(k) = i;
		pthread_create(&threads[i], NULL, create_histogram, k);

	}


	for(int i = 0; i < threads_num; i++){
		void* val;
		if(pthread_join(threads[i], &val) > 1) error_exit("Failed finishing thread");
		long thread_time = *((long*) val);

		printf("Thread %d took %ld ms\n", i+1, thread_time);
	}


	gettimeofday(&stop, NULL);

	long total_time = time_diff(start, stop);

	printf("TOTAL TIME: %ld\n", total_time);

	for(int i = 0; i < GRAY_SCALE_MAX; i++){
		for(int k = 1; k < threads_num; k++){
			histogram[0][i]+=histogram[k][i];
		}
	}

	save_to_file(output_file);

	free_arrays();

	printf("\n");

	exit(EXIT_SUCCESS);


}