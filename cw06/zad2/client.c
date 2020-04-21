#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"

int id = -1;
mqd_t c_queue = -1;
mqd_t s_queue = -1;
mqd_t chatting_queue = -1;
char chatting_name[NAME_LEN];
char name[NAME_LEN];
int received = 0;

void sigint_handler(int sig_number){
	exit(EXIT_SUCCESS);
}

void apply_nonblock(){
	struct mq_attr new, old;
	new.mq_flags = O_NONBLOCK;
	mq_getattr(c_queue, &old);
	mq_setattr(c_queue, &new, &old);
	mq_getattr(c_queue, &old);
}

void erase_nonblock(){
	struct mq_attr new, old;
	new.mq_flags = 0;
	mq_getattr(c_queue, &old);
	mq_setattr(c_queue, &new, &old);
	mq_getattr(c_queue, &old);
}
void handle_disconnect(){
	printf("In disconnect with chatting queue: %d\n", chatting_queue);
	if(mq_close(chatting_queue) == -1){
		printf("Could not close chatting queue. Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	chatting_queue = -1;
	strcpy(chatting_name, "");
	printf("Disconnected\n");
}

void disconnect(){
	erase_nonblock();
	char message[MAX_MSG_SIZE];
	sprintf(message, "%d", id);
	unsigned int priority = DISCONNECT;
	if(mq_send(s_queue, message, sizeof message, priority) == -1){
		printf("Could not send DISCONNECT to server. Error: %s\n", strerror(errno));
		return;
	}
	if(mq_receive(c_queue, message, sizeof message, &priority) == -1){
		printf("Could not receive DISCONNECT response from server. Error: %s\n", strerror(errno));
		return;
	}
	handle_disconnect();
}

void handle_exit(){

	if(s_queue != -1){
		if(mq_close(s_queue) == -1) printf("Could not close server queue\n");
		else printf("Server queue closed\n");
	}

	if(c_queue != -1){
		if(mq_close(c_queue) == -1) printf("Could not close client queue. Error %s\n", strerror(errno));
		else printf("Client queue closed\n");

		if(mq_unlink(name) == -1) printf("Could not delete client queue\n");
		else printf("Client queue deleted\n");
	}

}


void exit_function(){
	printf("=== EXIT ===\n");
	if(chatting_queue != -1){
		disconnect();
	}
	if(s_queue != -1) {
		char message[MAX_MSG_SIZE];
		sprintf(message, "%d", id);
		if(mq_send(s_queue, message, sizeof message, STOP) == -1){
			printf("Could not send message to server\n");
		}
	}

	handle_exit();
}

void set_queue_name(){

	srand(time(NULL));
	name[0] = '/';

	char a = 'a';
	char z = 'z';

	int range = (int) z - (int) a;

	for(int i = 1; i < NAME_LEN - 1; i++){
		name[i] = (char) ((int) a + rand() % range);
	}

	name[NAME_LEN - 1] = '\0';

}

void init(){

	char message[MAX_MSG_SIZE];
	unsigned int priority = INIT;
	strcpy(message, name);

	if(mq_send(s_queue, message, sizeof message, priority) == -1){
		printf("Could not send INIT response to server\n");
		exit(EXIT_FAILURE);
	}

	if(mq_receive(c_queue, message, sizeof message, &priority) == -1 || priority != INIT){
		printf("Could not receive INIT response from server\n");
		exit(EXIT_FAILURE);
	}

	char* ptr;
	id = strtol(message, &ptr, 10);

	printf("Initialized with ID %d\n", id);
}



void list(){
//	printf("=== LIST ===\n");

	erase_nonblock();

	char message[MAX_MSG_SIZE];
	unsigned int priority = LIST;
	sprintf(message, "%d", id);

	if(mq_send(s_queue, message, sizeof message, priority) == -1){
		printf("Could not send LIST to server. Error: %s\n", strerror(errno));
		return;
	}
	else{
		printf("Request LIST sent to server\n");
	}
	if(mq_receive(c_queue, message, sizeof message, &priority) == -1){
		printf("Could not receive LIST from server. Error: %s\n", strerror(errno));
		return;
	}

	printf("=== LIST ===\n%s\n", message);

}

void handle_connect(char* message){
	if(strcmp(message, "") == 0){
		printf("Connecting failed\n");
		return;
	}
	strcpy(chatting_name, message);

	if((chatting_queue = mq_open(message, O_WRONLY)) == -1){
		printf("Could not open chatting queue\n");
		return;
	}
	printf("Connected to %s\n", message);
}

void connect(int sec_id){
	erase_nonblock();

	char message[MAX_MSG_SIZE];
	sprintf(message, "%d %d", id, sec_id);
	unsigned int priority = CONNECT;
	if(mq_send(s_queue, message, sizeof message, priority) == -1){
		printf("Could not send CONNECT to server. Error: %s\n", strerror(errno));
		return;
	}
	if(mq_receive(c_queue, message, sizeof message, &priority) == -1){
		printf("Could not receive CONNECT response from server. Error: %s\n", strerror(errno));
		return;
	}

	handle_connect(message);
}

void send_chat_message(char* line){
	printf("Send: %s\n",line);
	char message[MAX_MSG_SIZE];
	strcpy(message, line);
	if(mq_send(chatting_queue, message, sizeof message, CHAT) == -1){
		printf("Could not send CHAT message. Error: %s\n", strerror(errno));
	}

}

void receive_msg(){

	apply_nonblock();

	char message[MAX_MSG_SIZE];
	strcpy(message, "");
	unsigned int priority;

	while(mq_receive(c_queue, message, sizeof message, &priority) >= 0) {
		switch (priority) {
			case STOP:
				printf("=== RECEIVED STOP ===\n");
				exit(EXIT_SUCCESS);
			case DISCONNECT:
				printf("=== RECEIVED DISCONNECT ===\n");
				handle_disconnect();
				break;
			case CONNECT:
				printf("=== RECEIVED CONNECT ===\n");
				handle_connect(message);
				break;
			case CHAT:
				printf("Received message: %s\n", message);
				break;
		}
		received = 1;
	}

}


int main(int argc, char** argv){

	if(signal(SIGINT, sigint_handler) == SIG_ERR){
		printf("Failed installing SIGINT handler\n");
		exit(EXIT_FAILURE);
	}

	if(atexit(exit_function) == -1){
		printf("Failed installing atexit function");
		exit(EXIT_FAILURE);
	}

	set_queue_name();

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	if((c_queue = mq_open(name, O_CREAT | O_EXCL | O_RDWR, 0666, &attr)) == -1){
		printf("Could not create server queue. Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if((s_queue = mq_open(S_QUEUE_NAME, O_WRONLY)) == -1){
		printf("Could not open server queue\n");
		exit(EXIT_FAILURE);
	}


	init();
	printf("My queue name: %s\n", name);

	char* list_str = "LIST";
	char* connect_str = "CONNECT";
	char* disconnect_str = "DISCONNECT";
	char* stop_str = "STOP";

	char line[MAX_MSG_SIZE];

	while(1){

		receive_msg();
		strcpy(line, "");
		fgets(line, sizeof line, stdin);

		if(strcmp(line, "\n") == 0) continue;

		if(strncmp(line, list_str, strlen(list_str)) == 0){
			printf("Getting list...\n");
			list();
		}
		else if(strncmp(line, connect_str, strlen(connect_str)) == 0 ){
			char* ptr;
			int sec_id = strtol(line + strlen(connect_str), &ptr, 10);
			connect(sec_id);
		}
		else if(strncmp(line, disconnect_str, strlen(disconnect_str)) == 0){
			if(chatting_queue == -1){
				printf("Cannot disconnect if you are not connected\n");
				continue;
			}
			disconnect();
		}
		else if(strncmp(line, stop_str, strlen(stop_str)) == 0){
			printf("Stopping...\n");
			exit(EXIT_SUCCESS);
		}
		else if(chatting_queue != -1){
			send_chat_message(line);
		}
		else{
			printf("Invalid entry. Use command or if you're connected just chat! \nAvailable commands:\n%s\n%s\n%s (only when connected)\n%s\n",
					list_str, connect_str, disconnect_str, stop_str);
		}
	}
}