#include"library.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/times.h>
#include <unistd.h>

struct saved_times{
	double user;
	double system;
	double real;
}; typedef struct saved_times saved_times;

saved_times* calc_times_diff(saved_times* saved_start, saved_times* saved_stop){
	saved_times* saved_diff = malloc(sizeof(saved_times));
	saved_diff->user = (saved_stop->user - saved_start->user)/sysconf(_SC_CLK_TCK);
	saved_diff->system = (saved_stop->system - saved_start->system)/sysconf(_SC_CLK_TCK);
	saved_diff->real = (saved_stop->real - saved_start->real)/sysconf(_SC_CLK_TCK);
	return saved_diff;
}

saved_times* save_times(saved_times* new_times, struct tms* time){
	new_times = malloc(sizeof(saved_times));
	new_times->real = times(time);
	new_times->system = time->tms_stime;
	new_times->user = time->tms_utime;
	return new_times;
}

void print_pointer_arr(pointer_arr* pointer_arr){
	for(int i = 0; i<pointer_arr->free_idx; i++){
		operation_block* operation_block = pointer_arr->operation_blocks[i];
		printf("%d",i);
		for(int j=0; j<operation_block->free_idx && operation_block->operations[i]!=NULL; j++){
			printf("%s", operation_block->operations[j]);
		}
	}
}

int main(int argc, char** argv){
	struct tms* start = malloc(sizeof(struct tms));
	struct tms* stop = malloc(sizeof(struct tms));

	saved_times* saved_start = save_times(saved_start, start);


    if(argc<3) {
        printf("Number of arguments must be at least 4.\n Give number of pairs, commend to compare pairs and at least one pair.");
    }

    int number_of_pairs = atoi(argv[1]);
    int given_pairs = 0;

    if(number_of_pairs == 0){
        printf("First argument should be a number of pairs of files");
    }

    pointer_arr* pointer_arr = create_pointer_arr(number_of_pairs);

	for(int i =1; i<argc; i++) {
		if (strcmp(argv[i], "compare_pairs") == 0) {
			while (++i < argc && strchr(argv[i], ':') != NULL) {
				given_pairs++;
				if(given_pairs>number_of_pairs){
					printf("More given pairs than declared");
					exit(EXIT_FAILURE);
				}
				char *file_a = strtok(argv[i], ":");
				char *file_b = strtok(NULL, ":");
				char *file_c = compare_and_write(file_a, file_b);
				insert_operation_block(file_c, pointer_arr);
			}
			i--;
		} else if (strcmp(argv[i], "remove_block") == 0) {
			if (++i < argc) {
				int idx = atoi(argv[i]);
				delete_operation_block(pointer_arr, idx);
			} else {
				printf("remove_block must be followed by block index");
				exit(EXIT_FAILURE);
			}
		} else if (strcmp(argv[i], "remove_operation") == 0) {
			if (i + 2 < argc) {
				int block_idx = atoi(argv[++i]);
				int operation_idx = atoi(argv[++i]);
				delete_operation(pointer_arr, block_idx, operation_idx);
			} else {
				printf("remove_operation must be followed by block index and operation index");
				exit(EXIT_FAILURE);
			}
		}
	}
	saved_times* saved_stop = save_times(saved_stop, stop);
	saved_times* times_diff =  calc_times_diff(saved_start, saved_stop);

	printf("USER --- SYS --- REAL\n");
	printf("%lf - %lf - %lf\n",
		   (double) times_diff->user,
		   (double) times_diff->system,
		   (double) times_diff->real);
	free(saved_stop);
	free(times_diff);
	free(start);
	free(stop);
	free(saved_start);

    delete_pointer_arr(pointer_arr);


}