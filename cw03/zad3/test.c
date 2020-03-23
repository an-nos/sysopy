#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>

#define MAX_ROW_LEN 10000
#define MAX_NUM_LEN 6

struct matrix{
	char* name;
	int** arr;
	int rows;
	int cols;
}; typedef struct matrix matrix;

struct list{
	matrix** As;
	matrix** Bs;
	matrix** Cs;
	int len;
}; typedef struct list list;

char* create_name(char* beg, int idx) {
	char* name = calloc(FILENAME_MAX, sizeof(char));
	strcpy(name, beg);
	char buff[100];
	snprintf(buff, 10, "%d", idx);
	strcat(name, buff);
	return name;
}

void print_names(char** names, int m_num) {
	for (int i = 0; i < m_num; i++) {
		printf("%s\n",names[i]);
	}
}

void print_array(int ** arr, int rows, int cols){
	for(int r = 0; r<rows; r++){
		for(int c = 0; c<cols; c++) printf("%d ",arr[r][c]);
		printf("\n");
	}
	printf("\n");
}

int** create_C_array(int rows, int cols){
	int** arr = calloc(rows, sizeof(int*));
	for(int r = 0; r < rows; r++){
		arr[r] = calloc(cols, sizeof(int));
	}
	return arr;
}

int** create_array(int rows, int cols){

	int** arr = calloc(rows, sizeof(int*));
	for(int r = 0; r < rows; r++){
		arr[r] = calloc(cols, sizeof(int));
		for(int c = 0; c < cols; c++){
			arr[r][c] = rand()%201 - 100;
		}
	}
	return arr;
}

int rand_val(int min, int max){
	return rand()%(max - min + 1) + min;
}

matrix* create_matrix(char* file_name, int rows, int cols, int is_C){
	matrix* M = malloc(sizeof(matrix));
	M->name = file_name;
	M->rows = rows;
	M->cols = cols;
	if(is_C == 0) M->arr = create_array(rows, cols);
	else M->arr = create_C_array(rows, cols);
	return M;
}

int calc_val_at(matrix* A, matrix* B, int r, int c){
	int res = 0;
	for(int i = 0; i<A->cols; i++) res+=A->arr[r][i]*B->arr[i][c];
	return res;
}

void multiply(matrix* A, matrix* B, matrix* C){
	for(int r = 0; r < C->rows; r++){
		for(int c = 0; c < C->cols; c++) C->arr[r][c] = calc_val_at(A, B, r, c);
	}
}

void save_to_file(matrix* M){
	FILE* f = fopen(M->name, "w+");
	char row[MAX_ROW_LEN];
	char val[MAX_NUM_LEN];
	for(int r = 0; r<M->rows; r++){
		strcpy(row, "");
		for(int c = 0; c<M->cols; c++){
			strcpy(val,"");
			sprintf(val, "%d", M->arr[r][c]);
			strcat(row, val);
			if(c != M->cols-1) strcat(row, " ");
		}
		strcat(row,"\n");
		strcat(row, "\0");
		fputs(row, f);
	}
	fclose(f);

}

void save_list(list* m_list){
	FILE* f = fopen("lista", "w+");
	char line[MAX_ROW_LEN];
	for(int i = 0; i<m_list->len; i++){
		strcpy(line, "");
		strcat(line, m_list->As[i]->name);
		strcat(line, " ");
		strcat(line, m_list->Bs[i]->name);
		strcat(line, " ");
		strcat(line, m_list->Cs[i]->name);
		strcat(line, "\n");
		strcat(line, "\0");
		fputs(line, f);
	}
	fclose(f);
}

matrix* read_matrix(int idx) {
	matrix* M = malloc(sizeof(matrix));
	M->name = create_name("C", idx);
	FILE *f = fopen(M->name, "r");
	char input[1024];
	char* val;
	char delimit[] = " \t\n";
	int rows = 1;
	int cols = 1;
	if(fgets(input, sizeof input, f)){
		strtok(input, delimit);
		while (strtok(NULL, delimit) != NULL) cols++;
	}
	while (fgets(input, sizeof input, f)) rows++;
	rewind(f);

	M->rows = rows;
	M->cols = cols;
	M->arr = calloc(rows, sizeof(int*));
	for(int r = 0; r<rows; r++) M->arr[r] = calloc(cols, sizeof(int));
	int r = 0;

	while (fgets(input, sizeof input, f)) {
		int c = 0;
		val = strtok(input, delimit);
		M->arr[r][c] = atoi(val);
		while (val != NULL && c<cols) {
			M->arr[r][c] = atoi(val);
			val = strtok(NULL, delimit);
			c++;
		}
		r++;
	}
	fclose(f);
	return M;
}

int compare_matrices(matrix* A, matrix* B){
	if(A->rows != B->rows || A->cols != B->cols) {
		printf("invalid sizes\n");
		return 0;
	}
	for(int r = 0; r<A->rows; r++){
		for(int c = 0; c<A->cols; c++){
			if(A->arr[r][c] != B->arr[r][c]) return 0;
		}
	}
	return 1;
}

void delete_matrix(matrix* M){
	for(int r = 0; r<M->rows; r++) free(M->arr[r]);
	free(M->arr);
	free(M->name);
	free(M);
}

int main(int argc, char** argv) {
	if(argc<4){
		printf("Invalid arguments. Expected: matrices_num min max\n");
		exit(EXIT_FAILURE);
	}
	int m_num = 3;
	int min = 1;
	int max = 5;
	srand(time(0));

	m_num = atoi(argv[1]);
	min = atoi(argv[2]);
	max = atoi(argv[3]);
	if(m_num == 0 || min == 0 || max == 0){
		printf("Invalid argument. Expected: matrices_num min max.\n");
		exit(EXIT_FAILURE);
	}
	list* m_list = malloc(sizeof(list));
	m_list->len = m_num;
	m_list->As = calloc(m_num, sizeof(matrix*));
	m_list->Bs = calloc(m_num, sizeof(matrix*));
	m_list->Cs = calloc(m_num, sizeof(matrix*));
	for(int i = 0; i<m_num; i++) {
		m_list->As[i] = create_matrix(create_name("A", i), rand_val(min, max), rand_val(min, max), 0);
		matrix* A = m_list->As[i];
		save_to_file(A);

		m_list->Bs[i] = create_matrix(create_name("B", i), A->cols, rand_val(min, max), 0);
		matrix* B = m_list->Bs[i];
		save_to_file(B);

		m_list->Cs[i] = create_matrix(create_name("C", i), A->rows, B->cols, 1);
	}

	save_list(m_list);

	for(int i = 0; i <m_list->len; i++){
		matrix* A = m_list->As[i];
		matrix* B = m_list->Bs[i];
		matrix* C = m_list->Cs[i];
		multiply(A, B, C);
	}

//	char command_p[FILENAME_MAX];
//	strcpy(command_p, "./main ");
//	strcat(command_p, "lista ");
//	strcat(command_p, "5 ");
//	strcat(command_p, "1000 ");
//	strcat(command_p, "NORMAL");
//	system(command_p);

	for(int i = 0; i<m_list->len; i++){
//		matrix* C = read_matrix(i);
//		if(compare_matrices(m_list->Cs[i], C) == 0){
//			printf("not equal");
//		}
//
//		delete_matrix(C);
		delete_matrix(m_list->Cs[i]);
		delete_matrix(m_list->As[i]);
		delete_matrix(m_list->Bs[i]);
	}

	free(m_list->As);
	free(m_list->Bs);
	free(m_list->Cs);
	free(m_list);

}

