#include "library.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LEN 1000

operation_block* create_operation_block(int difference_count){
    operation_block* operation_block = malloc(sizeof(operation_block));
    operation_block->difference_count = difference_count;
    operation_block->free_idx = 0;
    operation_block->operations = calloc(difference_count, sizeof(char*));
//    for(int i =0; i<operation_block->difference_count; i++){
//    	operation_block->operations[i]= NULL;
//    }
    return operation_block;
}

pointer_arr* create_pointer_arr(int block_count){
    pointer_arr* pointer_arr = malloc(sizeof(pointer_arr));
    pointer_arr->block_count = block_count;
    pointer_arr->free_idx = 0;
    pointer_arr->operation_blocks = calloc(block_count, sizeof(operation_block*));
//    for(int i =0; i<pointer_arr->block_count; i++){
//    	pointer_arr->operation_blocks[i] = NULL;
//    }
    return pointer_arr;
}

char* compare_and_write(char* file_a, char* file_b){

    char command[MAX_COMMAND_LEN] = "diff ";

    strcat(command, file_a);
    strcat(command, " ");
    strcat(command, file_b);
    strcat(command, " > file_c.txt");

    system(command);

    return "file_c.txt";

}

int insert_operation_block(char* diff_file_path, pointer_arr* pointer_arr){

    FILE* diff_file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;


    diff_file = fopen(diff_file_path,"r");
    if(diff_file == NULL)
        exit(EXIT_FAILURE);

    int operation_num = 0;

    while ((read = getline(&line, &len, diff_file)) != -1) {
        if(!(line[0] == '<' || line[0] == '>' || line[0] == '-')) operation_num++;
    }

	operation_block* operation_block = create_operation_block(operation_num);

	rewind(diff_file);
	operation_num = -1;

	while ((read = getline(&line, &len, diff_file)) != -1) {
		if(!(line[0] == '<' || line[0] == '>' || line[0] == '-')){
            operation_block->operations[++operation_num] = calloc(strlen(line),sizeof(char*));
		}
        strcat(operation_block->operations[operation_num],line);
	}
	operation_block->free_idx = operation_num+1;
    pointer_arr->operation_blocks[pointer_arr->free_idx++] = operation_block;

    fclose(diff_file);
    if (line)
        free(line);

    return pointer_arr->free_idx-1;
}

int operations_count(pointer_arr* pointer_arr, int idx){
	return pointer_arr->operation_blocks[idx]->free_idx;
}

void delete_operation(pointer_arr* pointer_arr, int block_idx, int operation_idx){
	operation_block* operation_block = pointer_arr->operation_blocks[block_idx];
	free(operation_block->operations[operation_idx]);

	for(int i = operation_idx; i < operation_block->free_idx - 1; i++){
		operation_block->operations[i] = operation_block->operations[i+1];
	}
	operation_block->free_idx--;
	operation_block->operations[operation_block->free_idx] = NULL;
}

void delete_operation_block(pointer_arr* pointer_arr, int block_idx){

	operation_block* operation_block = pointer_arr->operation_blocks[block_idx];

	for(int i = 0; i<operation_block->difference_count && operation_block->operations[i]!=NULL; i++){
		free(operation_block->operations[i]);
	}
	free(operation_block->operations);
	free(operation_block);

	for(int i = block_idx; i < pointer_arr->free_idx - 1; i++){
		pointer_arr->operation_blocks[i]=pointer_arr->operation_blocks[i+1];
	}
	pointer_arr->free_idx--;
}

void delete_pointer_arr(pointer_arr* pointer_arr){
	if(pointer_arr == NULL) return;
	if(pointer_arr->operation_blocks != NULL) {
		for (int i = 0; i < pointer_arr->free_idx; i++) {
			if (pointer_arr->operation_blocks[i] != NULL) delete_operation_block(pointer_arr, i);
		}
		free(pointer_arr->operation_blocks);
	}
	free(pointer_arr);
}