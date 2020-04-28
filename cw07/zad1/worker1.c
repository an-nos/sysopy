#include "common.h"


int get_random_number(){
	return 1 + rand()%RAND_RANGE;
}


void add(int sem_id, int orders_id){

	int order_value = get_random_number();

	struct sembuf sops[3];

	fill_sops_pos(sops, 0, IN_USE, 0, 0);	//wait until nobody modifies orders
	fill_sops_pos(sops, 1, ARE_FREE, 0, 0);	//wait until there are free places
	fill_sops_pos(sops, 2, IN_USE, 1, 0);	//mark orders as currently being used

	if(semop(sem_id, sops, 3) == -1) error_exit("Could not execute operations on semaphores.");

	orders* orders = shmat(orders_id, NULL, 0);

	orders->vals[orders->first_free] = order_value;
	orders->first_free = (orders->first_free + 1) % MAX_ORDERS;
	orders->num_to_prep++;

	printf("(%d %ld) Dostalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d\n",
			getpid(), time(NULL), order_value, orders->num_to_prep, orders->num_to_send);



	struct sembuf finalize[3];

	int semsop_idx = 0;


	if(orders->first_free == orders->first_to_send || orders->first_free == orders->first_to_prep){
		fill_sops_pos(finalize, semsop_idx++, ARE_FREE, 1, 0);	//there are no more free places
	}

	if(semctl(sem_id, 1, GETVAL, NULL) == 1){	//if there were no more to prepare before
		fill_sops_pos(finalize, semsop_idx++, ARE_TO_PREP, -1, 0);
	}

	fill_sops_pos(finalize, semsop_idx++, IN_USE, -1, 0);

	if(semop(sem_id, finalize, semsop_idx) == -1) error_exit("Could not execute operations on semaphores.");
	shmdt(orders);

}


int main(int argc, char** argv){


	signal(SIGINT, sigint_handler);

	srand(time(NULL));

	char cwd[PATH_MAX];
	getcwd(cwd, sizeof cwd);

	key_t sem_key = ftok(cwd, SEM_ID);

	int sem_id = semget(sem_key, 0, 0);
	if(sem_id == -1) error_exit("Could not access semaphores.");

	key_t ord_key = ftok(cwd, ORD_ID);
	int orders_id = shmget(ord_key, 0, 0);
	if(orders_id == -1) error_exit("Could not access shared memory.");


	while(1){
		usleep((rand()%5 + 1) * RAND_TIME_MUL);
		add(sem_id, orders_id);
	}


}