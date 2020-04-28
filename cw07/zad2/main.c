#include <wait.h>
#include "common.h"


const int workers_num = WORKER1_NUM + WORKER2_NUM + WORKER3_NUM;

pid_t worker_pids[WORKER1_NUM + WORKER2_NUM + WORKER3_NUM];
int sem_id;
int orders_id;


void exit_function(){

	for(int i = 0; i<workers_num; i++){
		kill(worker_pids[i], SIGINT);
	}

	sem_unlink(IN_USE);
	sem_unlink(ARE_TO_PREP);
	sem_unlink(ARE_TO_SEND);
	sem_unlink(ARE_FREE);

	if(shm_unlink(ORD_NAME) == -1) error_exit("Could not delete shared memory.");

}

sem_t* create_sem(char* name, int value){
	int oflag = O_RDWR | O_CREAT;

	sem_t* sem = sem_open(name, oflag, S_IRWXU | S_IRWXG | S_IRWXO, value);
	if(sem == SEM_FAILED) error_exit("Could not create semaphore.");

	return sem;
}

void create_sems(){
	sem_t* in_use_sem = create_sem(IN_USE, 1);
	sem_t* are_to_prep_sem = create_sem(ARE_TO_PREP, 0);
	sem_t* are_to_send_sem = create_sem(ARE_TO_SEND, 0);
	sem_t* are_free_sem = create_sem(ARE_FREE, 1);

	sem_close(in_use_sem);
	sem_close(are_to_prep_sem);
	sem_close(are_to_send_sem);
	sem_close(are_free_sem);
}

void create_orders(){
	int orders_fd;
	orders_fd = shm_open(ORD_NAME, O_RDWR | O_CREAT, 0666);
	if(orders_fd == -1) error_exit("Could not create shared memory.");

	if(ftruncate(orders_fd, sizeof(orders)) == -1) error_exit("Could not define size of shared memory.");

	orders* orders = mmap(NULL, sizeof(orders), PROT_READ | PROT_WRITE, MAP_SHARED, orders_fd, 0);

	orders->num_to_prep = orders->num_to_send = 0;
	orders->first_to_prep = orders->first_to_send = 0;
	orders->first_free = 0;

	for(int i = 0; i < MAX_ORDERS; i++){
		orders->vals[i] = -1;
	}

	if(munmap(orders, sizeof(orders)) == -1) error_exit("Could not unmount shared memory.");

}

int main(int argc, char** argv){

	atexit(exit_function);
	signal(SIGINT, sigint_handler);

	create_sems();

	create_orders();

	for(int i = 0; i < workers_num; i++){

		pid_t worker_pid = fork();

		if(worker_pid == 0) {
			if(i < WORKER1_NUM) execlp("./worker1", "worker1", NULL);
			else if (i < WORKER1_NUM + WORKER2_NUM) execlp("./worker2", "worker2", NULL);
			else execlp("./worker3", "worker3", NULL);
		}

		worker_pids[i] = worker_pid;

	}

	for(int i = 0; i < WORKER1_NUM; i++){
		wait(NULL);
	}
}