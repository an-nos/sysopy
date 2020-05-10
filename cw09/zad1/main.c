#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

int chairs_number = 0;
int clients_number = 0;
int waiting_clients = 0;
int active_clients = 0;
int is_barber_sleeping = 0;
pthread_t* chairs;
int last_idx = -1;
int first_free;

pthread_mutex_t chairs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_cond = PTHREAD_COND_INITIALIZER;


int rand_int(){
	return rand()%5 + 1;
}


void error_exit(char* message){
	printf("%s Error: %s\n", message, strerror(errno));
	exit(EXIT_FAILURE);
}


void* barber_routine(){

	pthread_mutex_lock(&chairs_mutex);

	while(waiting_clients == 0){
		is_barber_sleeping = 1;
		pthread_cond_wait(&barber_cond, &chairs_mutex);
	}
	is_barber_sleeping = 0;

	int current_client_idx = (last_idx + 1) % chairs_number;
	printf("Current client idx %d\n", current_client_idx);
	waiting_clients--;
	active_clients--;
	last_idx = current_client_idx;

	pthread_t current_client = chairs[current_client_idx];

	printf("Golibroda: czeka %d klientow, gole klienta %ld\n", waiting_clients, current_client);
	pthread_mutex_unlock(&chairs_mutex);
	sleep(rand_int());

//	pthread_cancel(current_client);


	if(active_clients > 0) barber_routine();

	pthread_exit(0);
}

void* client_routine(){
	int client_id = pthread_self();
	pthread_mutex_lock(&chairs_mutex);

	if(waiting_clients == chairs_number){

		printf("Zajete; %d\n", client_id);
		pthread_mutex_unlock(&chairs_mutex);

		sleep(rand_int());
		client_routine();
	}


	chairs[first_free] = client_id;
	first_free = (first_free + 1) % chairs_number;
	waiting_clients++;

	if(is_barber_sleeping){
		printf("Budze golibrode; %d\n",client_id);
		pthread_cond_broadcast(&barber_cond);
		pthread_mutex_unlock(&chairs_mutex);
	}
	else{
		printf("Poczekalnia, wolne miejsca: %d; %d\n", chairs_number - waiting_clients + 1, client_id);
		pthread_mutex_unlock(&chairs_mutex);
	}

	pthread_exit(0);
}

int main(int argc, char** argv){
	if(argc < 2) error_exit("Invalid arguments. Expected: chairs_number clients_number.");

	chairs_number = atoi(argv[1]);
	clients_number = atoi(argv[2]);

	if(chairs_number == 0) error_exit("Invalid chairs number.");
	if(clients_number == 0) error_exit("Invalid clients number");

	srand(time(NULL));

	chairs = calloc(chairs_number, sizeof(pthread_t));

	active_clients = clients_number;

	pthread_mutex_init(&chairs_mutex, NULL);
	pthread_cond_init(&barber_cond, NULL);


	pthread_t* threads = calloc(clients_number + 1, sizeof(pthread_t));

	pthread_create(&threads[clients_number], NULL, barber_routine, NULL);


	for(int i = 0; i < clients_number; i++){
		pthread_create(&threads[i], NULL, client_routine, NULL);
	}


	void* retval;
	for(int i = 0; i < clients_number + 1; i++){
		sleep(rand_int());
		pthread_join(threads[i], &retval);
	}

	pthread_mutex_destroy(&chairs_mutex);
	pthread_cond_destroy(&barber_cond);


}