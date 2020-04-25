#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/msg.h>
#include <string.h>
#include "common.h"

struct client{

	pid_t pid;
	key_t queue_key;

	int chatting_id;

}; typedef struct client client;


client clients[MAX_CLIENTS];
int s_queue_id = -1;


int get_first_free(){

	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].pid == -1) return i;
	}

	return -1;

}


const char* get_homedir(){
	struct passwd *pw = getpwuid(getuid());

	return pw->pw_dir;
}


void stop_client(int client_id){

	printf("=== STOP ID %d ===\n", client_id);

	msgbuf stop_msg;
	stop_msg.mtype = STOP;
	stop_msg.sender_id = getpid();
	stop_msg.receiver_id = client_id;

	int c_queue_id = msgget(clients[client_id].queue_key, 0);

	if(msgsnd(c_queue_id, &stop_msg, msgbuf_size, 0) == -1){
		printf("Could not stop client\n");
	}

	clients[client_id].pid = -1;
	clients[client_id].queue_key = -1;
	clients[client_id].chatting_id = -1;

	printf("Client %d removed\n", client_id);

}


void exit_all(){

	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].pid != -1) stop_client(i);
	}

}


void send_disconnect_response(int client_id){

	msgbuf response;
	response.sender_id = getpid();
	response.receiver_id = client_id;
	response.mtype = DISCONNECT;

	int c_queue_key = msgget(clients[client_id].queue_key, 0);

	if(msgsnd(c_queue_key, &response, msgbuf_size, 0) == -1){
		printf("Disconnecting %d failed", client_id);
		return;
	}

	clients[client_id].chatting_id = -1;

}

void disconnect(int client_id){
	printf("=== DISCONECT %d ===\n", client_id);

	int chatting_id = clients[client_id].chatting_id;

	send_disconnect_response(client_id);
	send_disconnect_response(chatting_id);

}


void list(int client_id){
	printf("=== LIST FOR ID %d ===\n", client_id);

	msgbuf list_msg;
	list_msg.mtype = LIST;
	list_msg.sender_id = getpid();
	list_msg.receiver_id = client_id;

	char* available = "AVAILABLE";
	char* busy = "BUSY";

	char text_buf[MAX_MSG_SIZE];
	strcpy(text_buf, "\0");
	strcpy(list_msg.text, "\0");

	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].pid == -1) continue;

		char* availability = (clients[i].chatting_id == -1) ? available : busy;
		if(i == client_id) availability = "YOU";

		sprintf(text_buf, "ID:\t%d\tAVAILABILITY:\t%s\n", i, availability);
		strcat(list_msg.text, text_buf);

	}

	int c_queue_key = msgget(clients[client_id].queue_key, 0);


	msgsnd(c_queue_key, &list_msg, msgbuf_size, 0);

}


void send_connect_msg(int client_id, int chatting_id){
	printf("== CONNECT %d TO %d ===\n", client_id, chatting_id);

	key_t chatting_key = (chatting_id == -1) ? -1 : clients[chatting_id].queue_key;

	msgbuf connect_msg;
	connect_msg.sender_id = client_id;
	connect_msg.receiver_id = chatting_id;
	connect_msg.mtype = CONNECT;
	sprintf(connect_msg.text, "%d", chatting_key);

	int c_queue_key = msgget(clients[client_id].queue_key, 0);

	if(msgsnd(c_queue_key, &connect_msg, msgbuf_size, 0) == -1){
		printf("Could not connect client of ID %d\n", client_id);
		return;
	}


	clients[client_id].chatting_id = chatting_id;

	if(chatting_id != -1)
		printf("Conencted %d to %d\n", client_id, chatting_id);

}



void connect(int sender_id, int receiver_id){

	if(sender_id == receiver_id || clients[sender_id].chatting_id != -1 || clients[receiver_id].chatting_id != -1){
		printf("Invalid connection request.\n");
		send_connect_msg(sender_id, -1);
		return;
	}

	send_connect_msg(sender_id, receiver_id);
	send_connect_msg(receiver_id, sender_id);

	if(clients[sender_id].chatting_id == -1 || clients[receiver_id].chatting_id == -1){
		clients[sender_id].chatting_id = clients[receiver_id].chatting_id = -1;
		printf("Connection between %d and %d failed\n", sender_id, receiver_id);
	}

}


void init(msgbuf msg){

	int id = get_first_free();

	printf("=== INIT %d ===\n", id);

	if(id == -1){
		printf("All client IDs are taken\n");
		return;
	}

	clients[id].pid = msg.sender_id;

	char* ptr;
	clients[id].queue_key = strtol(msg.text, &ptr, 10);

	msgbuf response;
	response.mtype = INIT;
	response.sender_id = getpid();
	response.receiver_id = -1;
	sprintf(response.text, "%d", id);


	int c_queue_id = msgget(clients[id].queue_key, 0);

	if(msgsnd(c_queue_id, &response, msgbuf_size, 0) == -1){
		printf("Failed sending a response to client\n");
		return;
	}

}

void exit_function(){
	printf("\n=== EXIT ===\n");
	exit_all();

	if(s_queue_id != -1 && msgctl(s_queue_id, IPC_RMID, 0) == -1){
		printf("Failed deleting sever queue\n");
		exit(EXIT_FAILURE);
	}
}

void sigint_handler(int sig_number) {
	exit(EXIT_SUCCESS);
}


int main (int argc, char** argv){

	atexit(exit_function);

	for (int i = 0; i < MAX_CLIENTS; i++){
		clients[i].pid = -1;
		clients[i].chatting_id = -1;
		clients[i].queue_key = -1;
	}

	if(signal(SIGINT, sigint_handler) == SIG_ERR){
		printf("Failed installing SIGINT handler\n");
		exit(EXIT_FAILURE);
	}


	key_t server_key = ftok(get_homedir(), PROJ_ID);

	if((s_queue_id = msgget(server_key, IPC_CREAT | 0666)) == -1){
		printf("Failed creating a server queue\n");
		exit(EXIT_FAILURE);
	}

	printf("Server initialized\n");


	msgbuf message_buffer;

	while(1){
		if(msgrcv(s_queue_id, &message_buffer, msgbuf_size, 0, 0) < 0) continue;
		switch(message_buffer.mtype){
			case STOP:
				stop_client(message_buffer.sender_id);
				break;
			case DISCONNECT:
				disconnect(message_buffer.sender_id);
				break;
			case LIST:
				list(message_buffer.sender_id);
				break;
			case CONNECT:
				connect(message_buffer.sender_id, message_buffer.receiver_id);
				break;
			case INIT:
				init(message_buffer);
				break;
		}
	}

}