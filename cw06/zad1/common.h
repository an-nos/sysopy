

#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#define PROJ_ID 'a'
#define MAX_MSG_SIZE 100
#define MAX_CLIENTS 10

enum msg_type{

	CHAT = 1,
	INIT = 2,
	CONNECT = 3,
	LIST = 4,
	DISCONNECT = 5,
	STOP = 6

};

struct msgbuf{
	long mtype;
	char text[MAX_MSG_SIZE];
	int sender_id;
	int receiver_id;
}; typedef struct msgbuf msgbuf;

const size_t msgbuf_size = sizeof(msgbuf) - sizeof(long);


#endif //SYSOPY_COMMON_H
