#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "common.h"


struct client{	//id of client is its
	char name[NAME_LEN];
	mqd_t queue;
	int chatting_id;
}; typedef struct client client;

mqd_t s_queue = -1;
client clients[MAX_CLIENTS];

int get_first_free(){

	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].queue == -1) return i;
	}

	return -1;

}

void sigint_handler(int sig_number){
	exit(EXIT_SUCCESS);
}

void exit_function(){
	printf("\n=== IN EXIT FUNCTION ===\n");
	char message[MAX_MSG_SIZE];
	strcpy(message, "");
	unsigned int priority = STOP;

	for(int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].queue != -1) {

			if (mq_send(clients[i].queue, message, sizeof message, priority) == -1) {
				printf("Could not send STOP message to client\n");
			}

			if (mq_close(clients[i].queue) == -1) {
				printf("Could not close client queue\n");
			}
		}
	}

	if(s_queue != -1){
		if(mq_close(s_queue) == -1){

			printf("\nCould not close server queue\n");
			printf("%s\n", strerror(errno));
		}


	}
	if(mq_unlink(S_QUEUE_NAME) == -1){
		printf("\nCould not delete server queue\n");
	}

}

void handle_stop(char* message){

	char* ptr;
	int id = strtol(message, &ptr, 10);

	if(mq_close(clients[id].queue) == -1){
		printf("Could not close client queue. Error: %s\n", strerror(errno));
	}

	clients[id].queue = -1;
	strcpy(clients[id].name, "");

}

void handle_init(char* message, int priority){
	printf("=== IN HANDLE INIT FUNCTION ===\n");

	int id = get_first_free();
	if(id < 0){
		printf("Could not connect new client, reason: too many clients\n");
		return;
	}

	strcpy(clients[id].name, message);

	if((clients[id].queue = mq_open(clients[id].name, O_WRONLY)) == -1){
		printf("Could not connect new client, reason: could not open client queue\n");
	}

	sprintf(message, "%d", id);

	mq_send(clients[id].queue, message, sizeof message, priority);
	printf("Client of ID %d initialized\n", id);

//	for(int i = 0; i < MAX_CLIENTS; i++){
//		printf("%d %d %s\n", i, clients[i].queue, clients[i].name);
//	}

}

void handle_list(char* message){

	char* ptr;
	int id = strtol(message, &ptr, 10);
	printf("=== IN HANDLE LIST FUNCTION ID %d ===\n", id);

	char text_buf[MAX_MSG_SIZE];
	char message_buf[MAX_MSG_SIZE];

	strcpy(text_buf, "");
	strcpy(message_buf, "");

	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].queue == -1) continue;

		char* availability = (clients[i].chatting_id == -1) ? "AVAILABLE" : "BUSY";
		if(i == id) availability = "YOU";

		sprintf(text_buf, "ID:\t%d\tAVAILABILITY:\t%s\n", i, availability);
		strcat(message_buf, text_buf);
	}

	printf("%s\n", message_buf);

	if(mq_send(clients[id].queue, message_buf, sizeof message_buf, LIST) == -1){
		printf("Could not send LIST reply to client. Error: %s\n", strerror(errno));
	}

}

void connect(int id1, int id2){

	char message[MAX_MSG_SIZE];

	strcpy(message, clients[id2].name);

	if(mq_send(clients[id1].queue, message, sizeof message, CONNECT) == -1){
		printf("Could not CONNECT %d to %d. Error: %s\n", id1, id2, strerror(errno));
	}
	else{
		printf("CONNECT sent from %d to %d\n", id1, id2);
	}
	clients[id1].chatting_id = id2;

}

void handle_connect(char* message){

	int id1 = atoi(strtok(message, " "));
	int id2 = atoi(strtok(NULL, " "));

	printf("id1 = %d, id2 = %d\n", id1, id2);

	if(id1 == id2 || clients[id1].chatting_id != -1 || clients[id2].chatting_id != -1){
		printf("Invalid connection\n");
		char message[MAX_MSG_SIZE];
		strcpy(message, "");
		if(mq_send(clients[id1].queue, message, sizeof message, CONNECT) != -1){
			printf("Could not reply to CONNECT. Error: %s\n", strerror(errno));
		}
		return;
	}
	connect(id1, id2);
	connect(id2, id1);

}

void disconnect(int id){

	char message[MAX_MSG_SIZE];
	strcpy(message,"");

	if(mq_send(clients[id].queue, message, sizeof message, DISCONNECT) == -1){
		printf("Could not send DISCONNECT to %d. Error: %s\n",id, strerror(errno));
		return;
	}

}

void handle_disconnect(char* message){
	int id = atoi(message);
	disconnect(id);
	disconnect(clients[id].chatting_id);
	clients[clients[id].chatting_id].chatting_id = -1;
	clients[id].chatting_id = -1;

}

int main(int argc, char** argv){

	atexit(exit_function);

	if(signal(SIGINT, sigint_handler) == SIG_ERR){
		printf("Failed installing SIGINT handler\n");
		exit(EXIT_FAILURE);
	}


	for(int i = 0; i < MAX_CLIENTS; i++){
		clients[i].queue = -1;
		clients[i].chatting_id = -1;
		strcpy(clients[i].name, "");
	}

	struct mq_attr attr;

    attr.mq_flags = O_NONBLOCK;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	if((s_queue = mq_open(S_QUEUE_NAME, O_CREAT | O_EXCL | O_RDWR, 0666, &attr)) == -1){
		printf("Could not create server queue. Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}



	printf("Server initialized\n");

	char message_buf[MAX_MSG_SIZE];
	unsigned int priority;

	while(1){

		if(mq_receive(s_queue, message_buf, sizeof message_buf, &priority) == -1){
			printf("Receiving error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		switch(priority){
			case STOP:
				printf("=== RECEIVED STOP ===\n");
				handle_stop(message_buf);
				break;
			case DISCONNECT:
				printf("=== RECEIVED DISCONNECT ===\n");
				handle_disconnect(message_buf);
				break;
			case LIST:
				printf("=== RECEIVED LIST ===\n");
				handle_list(message_buf);
				break;
			case CONNECT:
				printf("=== RECEIVED CONNECT ===\n");
				handle_connect(message_buf);
				break;
			case INIT:
				printf("=== RECEIVED INIT ===\n");
				handle_init(message_buf, priority);
				break;
		}

	}


}