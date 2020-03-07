#ifndef LAB1ZAD1_LIBRARY_H
#define LAB1ZAD1_LIBRARY_H

struct operation_block{
    char** operations;
    int difference_count;
    int free_idx;
}; typedef struct operation_block operation_block;

struct pointer_arr{
    operation_block** operation_blocks;
    int operation_count;
    int free_idx;
}; typedef struct pointer_arr pointer_arr;


pointer_arr* create_pointer_arr(int operation_count);

char* compare_and_write(char* file_a, char* file_b);

int insert_operation_block(char* diff_file_path, pointer_arr* pointer_arr);

int operations_count(pointer_arr* pointer_arr, int idx);

void delete_operation(pointer_arr* pointer_arr, int block_idx, int operation_idx);

void delete_operation_block(pointer_arr* pointer_arr, int block_idx);

void print_pointer_arr(pointer_arr* pointer_arr);

#endif //LAB1ZAD1_LIBRARY_H
