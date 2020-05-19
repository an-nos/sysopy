#include "common.h"

int is_local;
int port_no;
char* server;
char* nick;
int server_fd;
char symbol;
int move;

void disconnect_from_server(){
	printf("Disconnecting from server...\n");
	send_message(server_fd, DISCONNECT, NULL, nick);
	if(is_local) unlink(nick);
	exit(EXIT_SUCCESS);
}

void sigint_handler_client(int signo){
	printf("Closing client...\n");
	exit(EXIT_SUCCESS);
}

void at_exit_fun(){
	disconnect_from_server();
}

void local_connect_to_server(){

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, server);

	server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(server_fd < 0) error_exit("Socket to server failed.");

	struct sockaddr_un c_addr;
	c_addr.sun_family = AF_UNIX;
	strcpy(c_addr.sun_path, nick);


	if(bind(server_fd, (struct sockaddr*) &c_addr, sizeof(c_addr)) < 0)
		error_exit("Bind failed.");

	if(connect(server_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		error_exit("Connect to server failed.");

}

void inet_connect_to_server(){

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_no);
	addr.sin_addr.s_addr = inet_addr(server);

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(server_fd < 0) error_exit("Socket to server failed.");

	struct sockaddr_in c_addr;
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = 0;
	c_addr.sin_addr.s_addr = inet_addr(server);

	if(bind(server_fd, (struct sockaddr*) &c_addr, sizeof(c_addr))<0)
		error_exit("Bind failed.");

	if(connect(server_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		error_exit("Connect to server failed.");
}


void print_gameboard(game* game){
	printf("BOARD\n\n");
	for(int i = 0; i < 9; i++){
		printf("%c",game->board[i]);
		if(i % 3 == 2) printf("\n");
	}
	printf("\n");
}

void print_instruction(){
	printf("To make a move, simply write a number corresponding to the field.\n");
	printf("The game board is represented by numbers as follows:\n");
	for(int i = 0; i < 9; i++){
		printf("%d",i);
		if(i % 3 == 2) printf("\n");
	}
	printf("\n");
}

void concurrent_move(void* arg){
	message* msg = (message*) arg;
	printf("Enter your move: ");

	int move_char = getchar();
	move = move_char - '0';

	while(move < 0 || move > 8 || msg->game.board[move] != '-'){
//		printf("Field is invalid, try again.\n");
		move_char = getchar();
		move = move_char - '0';
	}
	pthread_exit(0);
}

void make_move(message *msg){

	move = -1;

	pthread_t move_thread;
	pthread_create(&move_thread, NULL, (void*) concurrent_move, msg);

	while(move == -1){
		message rec_msg = receive_message_nonblock(server_fd);
		switch(rec_msg.message_type){
			case PING:
				printf("Received PING from server. Pinging back...\n");
				send_message(server_fd, PING, NULL, nick);
				break;
			case DISCONNECT:
				printf("Received DISCONNECT from server.\n");
				sigint_handler_client(SIGINT);
				exit(EXIT_SUCCESS);
			case EMPTY:
				break;
			default:
				printf("Wrong message received\n");
				break;
		}
	}

	pthread_join(move_thread, NULL);
	printf("Your move was: %d\n", move);

	msg->game.board[move] = symbol;
	print_gameboard(&msg->game);

	send_message(server_fd, MOVE, &msg->game, nick);

}



void client_routine(){

	for( ; ; ){
		message msg = receive_message(server_fd);
		switch(msg.message_type){
			case WAIT:
				printf("Waiting for an opponent.\n");
				break;
			case GAME_FOUND:
				symbol = msg.game.winner;
				printf("Game started. Your symbol: %c\n", symbol);
				print_gameboard(&msg.game);
				if(symbol == 'O') make_move(&msg);
				else printf("Waiting for enemy move\n");
				break;
			case MOVE:
				printf("MOVE!\n");
				print_gameboard(&msg.game);
				make_move(&msg);
				break;
			case GAME_FINISHED:
				print_gameboard(&msg.game);
				if(msg.game.winner == symbol) printf("YOU WON!\n");
				else if(msg.game.winner == 'D') printf("IT'S A DRAW!\n");
				else printf("YOU LOST. REALLY?\n");
				disconnect_from_server();
				exit(EXIT_SUCCESS);
				break;
			case PING:
				printf("Received PING from server. Pinging back...\n");
				send_message(server_fd, PING, NULL, nick);
				break;
			case DISCONNECT:
				printf("Received DISCONNECT from server.\n");
				sigint_handler_client(SIGINT);
				exit(EXIT_SUCCESS);
			default: break;
		}

	}

}

int main(int argc, char** argv){

	if(argc < 4) error_exit("Invalid arguments. Expected: nick LOCAL/INET server_address");

	nick = argv[1];

	if(strcmp(argv[2], "LOCAL") == 0) is_local = 1;
	else if(strcmp(argv[2], "INET") == 0) is_local = 0;
	else  error_exit("Invalid arguments. Expected: nick LOCAL/INET server_address");


	if(is_local){
		server = argv[3];
	}
	else{
		if(argc < 5) error_exit("Invalid arguments. Expected: nick INET IP_adress port_no");
		server = argv[3];
		port_no = atoi(argv[4]);
		printf("server IP: %s port: %d\n",server, port_no);
	}

	atexit(at_exit_fun);
	signal(SIGINT, sigint_handler_client);

	if(is_local) local_connect_to_server();
	else inet_connect_to_server();

	print_instruction();

	send_message(server_fd, CONNECT, NULL, nick);

	message msg = receive_message(server_fd);

	if(msg.message_type == CONNECT){
		printf("Connected to server\n");
		client_routine();
	}
	if(msg.message_type == CONNECT_FAILED){
		printf("Connect failed. %s\n",msg.nick);
		if(shutdown(server_fd, SHUT_RDWR) < 0)
			error_exit("Could not shutdown.");
		if(close(server_fd) < 0)
			error_exit("Could not close server descriptor.");
		error_exit("");
	}

	printf("Something went wrong\n");
	disconnect_from_server();

}