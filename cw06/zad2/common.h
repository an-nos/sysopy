#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#define MAX_MSG_SIZE 100
#define MAX_CLIENTS 10
#define NAME_LEN 8
#define MAX_MSG 20

const char S_QUEUE_NAME[] = "/serverqueue";

enum msg_type{

	CHAT = 1,
	INIT = 2,
	CONNECT = 3,
	LIST = 4,
	DISCONNECT = 5,
	STOP = 6

};



#endif //SYSOPY_COMMON_H
