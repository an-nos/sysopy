#include"library.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

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
    if(argc<3) {
        printf("Number of arguments must be at least 4.\n Give number of pairs, commend to compare pairs and at least one pair.");
        exit(EXIT_FAILURE);
    }

    int number_of_pairs = atoi(argv[1]);

    if(number_of_pairs == 0){
        printf("First argument should be a number of pairs of files");
    }

    pointer_arr* pointer_arr = create_pointer_arr(number_of_pairs);

    for(int i =1; i<argc; i++){
        if(strcmp(argv[i], "compare_pairs") == 0){
            while(++i<argc && strchr(argv[i],':') != NULL){
                char* file_a = strtok(argv[i], ":");
                char* file_b = strtok(NULL, ":");
                char* file_c = compare_and_write(file_a, file_b);
                insert_operation_block(file_c, pointer_arr);
            }
            i--;
        }
        else if(strcmp(argv[i], "remove_block") == 0){
            if(++i<argc) {
                int idx = atoi(argv[i]);
                delete_operation_block(pointer_arr, idx);
            }
            else{
                printf("remove_block must be followed by block index");
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i],"remove_operation") == 0){
            if(i+2<argc){
                int block_idx = atoi(argv[++i]);
                int operation_idx = atoi(argv[++i]);
                delete_operation(pointer_arr, block_idx, operation_idx);
            }
            else{
                printf("remove_operation must be followed by block index and operation index");
                exit(EXIT_FAILURE);
            }
        }

    }
    print_pointer_arr(pointer_arr);

}