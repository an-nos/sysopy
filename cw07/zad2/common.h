#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H


#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define MAX_ORDERS 10

#define ORD_NAME "shared"
#define IN_USE "inuse"
#define ARE_TO_PREP "aretoprep"
#define ARE_TO_SEND "aretosend"
#define ARE_FREE "arefree"

#define NSEMS 4
#define RAND_RANGE 100
#define RAND_TIME_MUL 100000
#define WORKER1_NUM 2
#define WORKER2_NUM 2
#define WORKER3_NUM 2

struct orders{
	int first_free;

	int num_to_prep;
	int first_to_prep;

	int num_to_send;
	int first_to_send;


	int vals[MAX_ORDERS];
}; typedef struct orders orders;

void error_exit(char* message){
	printf("%s Error: %s\n", message, strerror(errno));
	exit(EXIT_FAILURE);
}

void sigint_handler(int signo){
	exit(EXIT_SUCCESS);
}

sem_t* open_sem(char* name){
	sem_t* sem = sem_open(name, O_RDWR);
	if(sem == SEM_FAILED) error_exit("Could not create semaphore.");
	return sem;
}

#endif //SYSOPY_COMMON_H
