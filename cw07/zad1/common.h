#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define MAX_ORDERS 10

#define SEM_ID 'a'

#define ORD_ID 'b'

#define NSEMS 4
#define RAND_RANGE 100
#define RAND_TIME_MUL 100000
#define WORKER1_NUM 5
#define WORKER2_NUM 2
#define WORKER3_NUM 3

struct orders{
	int first_free;

	int num_to_prep;
	int first_to_prep;

	int num_to_send;
	int first_to_send;


	int vals[MAX_ORDERS];
}; typedef struct orders orders;

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

enum sem_help{

	IN_USE = 0,
	ARE_TO_PREP = 1,
	ARE_TO_SEND = 2,
	ARE_FREE = 3

};

void error_exit(char* message){
	printf("%s Error: %s\n", message, strerror(errno));
	exit(EXIT_FAILURE);
}

void sigint_handler(int signo){
	exit(EXIT_SUCCESS);
}

void fill_sops_pos(struct sembuf* sops, int idx, int num, int op, int flg){
	sops[idx].sem_num = num;
	sops[idx].sem_op = op;
	sops[idx].sem_flg = flg;
}


#endif //SYSOPY_COMMON_H
