#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <string.h>
#include "common.h"

int client_id = -1;
int chatting_id = -1;
int s_queue_id = -1;
int c_queue_id = -1;
int chatting_queue_id = -1;

const char* get_homedir(){
	struct passwd *pw = getpwuid(getuid());
	return pw->pw_dir;
}

void delete_queue(){

	printf("Deleting client queue...\n");

	if(msgctl(c_queue_id, IPC_RMID, 0) == -1){
		printf("Failed deleting client queue\n");
		exit(EXIT_FAILURE);
	}

}

void disconnect(){

	msgbuf disconnect_msg;
	disconnect_msg.mtype = DISCONNECT;
	disconnect_msg.sender_id = client_id;
	disconnect_msg.receiver_id = chatting_id;

	if(msgsnd(s_queue_id, &disconnect_msg, msgbuf_size, 0) == -1){
		printf("Failed sending disconnect message to chat\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	if(msgrcv(c_queue_id, &disconnect_msg, msgbuf_size, 0, 0) == -1 || disconnect_msg.mtype != DISCONNECT){
		printf("Failed disconnecting from chat\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	chatting_id = -1;
	chatting_queue_id = -1;

	printf("Disconnected\n");
}

void stop(){

	if(chatting_queue_id != -1){
		printf("Disconnecting...\n");
		disconnect();
	}

	msgbuf stop_msg;
	stop_msg.mtype = STOP;
	stop_msg.sender_id = client_id;

	if(msgsnd(s_queue_id, &stop_msg, msgbuf_size, 0) == -1){
		printf("Could not send stop message\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	if(msgrcv(c_queue_id, &stop_msg, msgbuf_size, 0, 0) == -1 || stop_msg.mtype != STOP){
		printf("Failed receiving STOP message from server\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	printf("Succesfully deleted from server. Client shutting down...\n");
	delete_queue();
	exit(EXIT_SUCCESS);
}

void sigint_handler(int signum){
	stop();
	exit(EXIT_SUCCESS);
}


void connect_to(int id, char* key_str){
	chatting_id = id;

	char* ptr;
	key_t chatting_key = strtol(key_str, &ptr, 10);

	chatting_queue_id = msgget(chatting_key, 0);

	printf("Connected to %d\n", id);
}

void receive_msg(){

	msgbuf message;

	while(msgrcv(c_queue_id, &message, msgbuf_size, 0, IPC_NOWAIT) >= 0) {
		switch (message.mtype) {
			case STOP:
				if (chatting_queue_id != -1) disconnect();
				delete_queue();
				exit(EXIT_SUCCESS);
			case DISCONNECT:
				chatting_queue_id = -1;
				chatting_id = -1;
				printf("Disconnected\n");
				break;
			case CONNECT:
				connect_to(message.receiver_id, message.text);
				break;
			case CHAT:
				printf("%s", message.text);
				break;
		}
	}
}

void list(){
	msgbuf list_msg;
	list_msg.sender_id = client_id;
	list_msg.receiver_id = -1;
	list_msg.mtype = LIST;
	strcpy(list_msg.text, "\0");

	if(msgsnd(s_queue_id, &list_msg, msgbuf_size, 0) == -1){
		printf("Could not send LIST message to server\n");
		return;
	}

	if(msgrcv(c_queue_id, &list_msg, msgbuf_size, 0, 0) == -1){
		printf("Could not receive LIST response from server\n");
		return;
	}

	printf("%s", list_msg.text);

}


void connect(int id){

	msgbuf connect_msg;
	connect_msg.mtype = CONNECT;
	connect_msg.sender_id = client_id;
	connect_msg.receiver_id = id;

	if(msgsnd(s_queue_id, &connect_msg, msgbuf_size, 0) == -1){
		printf("Failed connecting to %d\n", id);
		return;
	}

	if(msgrcv(c_queue_id, &connect_msg, msgbuf_size, 0, 0) == -1){
		printf("Failed receiving connect response\n");
		return;
	}

	connect_to(connect_msg.receiver_id, connect_msg.text);


}


void init(key_t client_key){

	msgbuf init_msg;
	init_msg.mtype = INIT;
	init_msg.sender_id = getpid();
	init_msg.receiver_id = -1;
	sprintf(init_msg.text, "\t%d", client_key);


	if(msgsnd(s_queue_id, &init_msg, msgbuf_size, 0) == -1){
		printf("Failed sending init to server\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	if(msgrcv(c_queue_id, &init_msg, msgbuf_size, 0, 0) == -1){
		printf("Failed receiving init response from server\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	if(init_msg.mtype != INIT){
		printf("Expected init response\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}

	char* ptr;

	client_id = strtol(init_msg.text, &ptr, 10);

	printf("Initialized successfully\n");

}

void send_chat_message(char* text){

	msgbuf chat_msg;
	chat_msg.mtype = CHAT;
	chat_msg.sender_id = client_id;
	chat_msg.receiver_id = chatting_id;
	strcpy(chat_msg.text, text);

	if(msgsnd(chatting_queue_id, &chat_msg, msgbuf_size, 0) == -1){
		printf("Could not send message to %d", chatting_id);
		return;
	}

}



int main (int argc, char** argv){

	if(signal(SIGINT, sigint_handler) == SIG_ERR){
		printf("Failed installing SIGINT handler\n");
		exit(EXIT_FAILURE);
	}

	key_t client_key = ftok(get_homedir(), getpid());

	if((c_queue_id = msgget(client_key, IPC_CREAT | 0666)) == -1){
		printf("Failed creating a client queue\n");
		exit(EXIT_FAILURE);
	}

//	printf("KEY: %d\n", client_key);
//	printf("QUEUE: %d\n", c_queue_id);


	key_t server_key = ftok(get_homedir(), PROJ_ID);

	if((s_queue_id = msgget(server_key, 0)) == -1){
		printf("Could not connect to server\n");
		delete_queue();
		exit(EXIT_FAILURE);
	}


	init(client_key);


	if(signal(SIGINT, sigint_handler) == SIG_ERR){
		printf("Failed installing SIGINT handler\n");
		exit(EXIT_FAILURE);
	}

	char* list_str = "LIST";
	char* connect_str = "CONNECT";
	char* disconnect_str = "DISCONNECT";
	char* stop_str = "STOP";

	char line[MAX_MSG_SIZE];
	while(fgets(line, sizeof line, stdin)){

		receive_msg();

		if(strncmp(line, list_str, strlen(list_str)) == 0){
			list();
		}
		else if(strncmp(line, connect_str, strlen(connect_str)) == 0 ){
			char* ptr;
			int id = strtol(line + strlen(connect_str), &ptr, 10);
			connect(id);
		}
		else if(strncmp(line, disconnect_str, strlen(disconnect_str)) == 0){
			if(chatting_queue_id == -1){
				printf("Cannot disconnect if you are not connected\n");
				continue;
			}
			disconnect();
		}
		else if(strncmp(line, stop_str, strlen(stop_str)) == 0){
			stop();
		}
		else if(chatting_id != -1){
			send_chat_message(line);
		}
		else{
			printf("Invalid entry. Use command or if you're connected just chat! \n Available commands:\n%s\n%s\n%s (only when connected)\n%s\n",
					list_str, connect_str, disconnect_str, stop_str);
		}
	}
}