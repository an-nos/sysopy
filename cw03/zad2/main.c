#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include <unistd.h>


struct matrix{
	char* file_name;
	int rows;
	int cols;
	int chars_per_num;
}; typedef struct matrix matrix;


struct list{
	matrix** As;
	matrix** Bs;
	matrix** Cs;
	int len;
}; typedef struct list list;

#define MIN_VAL 100
#define MAX_VAL 100
#define MAX_ROW_LEN 1000

int get_idx(matrix* M, int r, int c){
	return M->chars_per_num*M->cols*r + c*M->chars_per_num;
}

void count_rows(matrix* M) {
	FILE* f = fopen(M->file_name, "r");
	char str[FILENAME_MAX];
	int i = 0;
	while(fgets(str, FILENAME_MAX, f) != NULL) i++;
	M->rows = i;
	fclose(f);
}

void count_columns(matrix* M) {
	FILE* f = fopen(M->file_name, "r");
	char* str = calloc(MAX_ROW_LEN, sizeof(char));
	fgets(str, MAX_ROW_LEN, f);
	int i = 1;
	strtok(str, " ");
	while(strtok(NULL, " ") != NULL) i++;
	M->cols = i;
	free(str);
	fclose(f);
}

int get_val_at(matrix* M, int r, int c){
	FILE* f = fopen(M->file_name, "r");
	int i = 0;
	char* str = calloc(MAX_ROW_LEN, sizeof(char));
	while(fgets(str, MAX_ROW_LEN, f) != NULL && i<r && i<M->rows) i++;
	i = 0;
	char* token;
	token = strtok(str, " ");
	while(token != NULL && i<c && i<M->cols){
		token = strtok(NULL, " ");
		i++;
	}
//	char *ptr;
	int ret;
	if(token == NULL){
		printf("Couldn't find given position\n");
		exit(EXIT_FAILURE);
	}
	ret = atoi(token);
	free(str);
	fclose(f);
	return ret;
}

matrix* create_matrix(matrix* A, matrix* B, char* file_name){
	matrix* C = malloc(sizeof(matrix));
	C->file_name = calloc(FILENAME_MAX, sizeof(char));
	strcpy(C->file_name, file_name);
	C->rows = A->rows;
	C->cols = B->cols;
	FILE* f = fopen(C->file_name, "w+");
	char* space = "#";
	char* endl = "\n";
	C->chars_per_num = (int) log10(MAX_VAL*MIN_VAL) + 3;	//+1 for sign, +1 because log100=2, +1 for a space after
	int r_len = C->chars_per_num*C->cols;
	for(int j =0; j<C->rows; j++){
		for(int i = 0; i<r_len - 1; i++) {
			fwrite(space, 1, 1, f);
		}
		fwrite(endl, 1, 1, f);
	}
	fclose(f);
	return C;
}

matrix* read_matrix(char* file_name){
	matrix* M = malloc(sizeof(matrix));
	M->file_name = calloc(FILENAME_MAX, sizeof(char));
	strcpy(M->file_name, file_name);
	count_columns(M);
	count_rows(M);
	return M;
}

void write_in_pos(matrix *M, int r, int c, int val){
	FILE* f = fopen(M->file_name, "r+");
	int idx = get_idx(M, r, c);
	char* num = calloc(M->chars_per_num, sizeof(char));
	sprintf(num, "%d", val);
	int digs =floor(log10(abs(val))) + 1;
	if(val < 0) digs++;
	for(int i = digs; i<M->chars_per_num -1; i++){
		num[i] = '#';
	}
	fseek(f, idx, 0);
	fwrite(num, sizeof(char), M->chars_per_num -1, f);
	free(num);
	fclose(f);
}

void multiply_col(matrix* A, matrix* B, matrix* C, int c){
	for(int r = 0; r<A->rows; r++){
		int res = 0;
		for(int i = 0; i<A->cols; i++){
			res+=get_val_at(A, r, i)* get_val_at(B, i,c);
		}
		write_in_pos(C,r,c,res);
	}
}

list* read_list(char* file_name){
	FILE* f = fopen(file_name, "r");
	char* line = calloc(MAX_ROW_LEN, sizeof(char));
	int len = 0;
	while(fgets(line, MAX_ROW_LEN, f) != NULL) len++;
	list* m_list = (list*) malloc(sizeof(list));
	m_list->As = (matrix**) calloc(len, sizeof(matrix*));
	m_list->Bs = (matrix**) calloc(len, sizeof(matrix*));
	m_list->Cs = (matrix**) calloc(len, sizeof(matrix*));
	m_list->len = len;
	rewind(f);
	int i = 0;
	while(fgets(line, MAX_ROW_LEN, f) != NULL){
		char* file_A = strtok(line, " ");
		char* file_B = strtok(NULL, " ");
		char* file_C = strtok(NULL, "\n");
		m_list->As[i] = read_matrix(file_A);
		m_list->Bs[i] = read_matrix(file_B);
		m_list->Cs[i] = create_matrix(m_list->As[i], m_list->Bs[i], file_C);
		printf("%s\n",file_C);
		i++;
	}
	free(line);
	fclose(f);
	return m_list;
}

void multiply(list* m_list, int proc_idx, int proc_count){
	printf("In multiply proc %d\n", proc_idx);
	for(int i = 0; i<m_list->len; i++){
//		printf("%d matrices\n",i);
		int cols_per_proc;
		int start_col;
		if(proc_idx>=m_list->Bs[i]->cols) continue;
//		if(proc_idx == m_list->Bs[i]->cols - 1){
//			cols_per_proc = 1;
//			multiply_col(m_list->As[i], m_list->Bs[i], m_list->Cs[i], proc_idx);
//		}
		if(proc_count > m_list->Bs[i]->cols){
			cols_per_proc=1;
			start_col=proc_idx;
		}
		else{
			cols_per_proc = m_list->Bs[i]->cols/proc_count;
			start_col = cols_per_proc*proc_idx;
			int rem = m_list->Bs[i]->cols - cols_per_proc*(proc_idx + 1);
//			if(rem == 0) continue;
			if(rem != 0 && rem < cols_per_proc){
				cols_per_proc = rem;
			}
//			printf("cols_per_proc %d\n",cols_per_proc);
		}
		for(int c = start_col; c < start_col+cols_per_proc; c++){
//				printf("col %d\n",c);
			multiply_col(m_list->As[i], m_list->Bs[i], m_list->Cs[i], c);
		}
	}
}



int main(int argc, char** argv) {
	list* m_list = read_list("lista");
//	printf("%d\n",m_list->len);

	int proc_count=2;
	pid_t* child_pids = calloc(proc_count, sizeof(pid_t));

	for(int i =0; i<proc_count; i++){
		pid_t child_pid = fork();
		if(child_pid == 0){
			multiply(m_list, i, proc_count);
		}
		else{
			child_pids[i] = child_pid;
		}
	}
}