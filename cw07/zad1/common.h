#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

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


#endif //SYSOPY_COMMON_H
