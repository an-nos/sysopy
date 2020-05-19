#include "common.h"

#define PING_INTERVAL 10
#define PING_WAIT 5

int port_no;
char* socket_path;

struct sockaddr local_sockaddr;
struct sockaddr_in inet_sockaddr;

int local_sock;
int inet_sock;

client clients[MAX_CLIENTS];
int waiting_idx;
int first_free;

pthread_mutex_t clients_mutex;

pthread_t net_thread;
pthread_t ping_thread;

int is_client(int i){
	return i < MAX_CLIENTS && clients[i].fd != -1;
}


int get_client_index(char* nick){
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(is_client(i) && strcmp(nick, clients[i].nick) == 0) return i;
	}
	return -1;
}

void close_server(){

	if(pthread_cancel(net_thread) == -1) error_exit("Could not cancel net tread");
	if(pthread_cancel(ping_thread) == -1) error_exit("Could not cancel ping thread");

	close(local_sock);
	unlink(socket_path);
	close(inet_sock);
}

void sigint_handler_server(int signo){
	printf("Closing server...");
	close_server();
}

void start_local(){
	int local_domain = AF_UNIX;

	local_sock = socket(local_domain, SOCK_DGRAM, 0);
	if(local_sock == -1) error_exit("Local socket initialization failed.");

	local_sockaddr.sa_family = local_domain;
	strcpy(local_sockaddr.sa_data, socket_path);

	int local_bind = bind(local_sock, &local_sockaddr, sizeof(local_sockaddr));
	if(local_bind == -1) error_exit("Local bind failed.");


	printf("Local socket fd: %d\n", local_sock);

}

void start_inet(){
	int inet_domain = AF_INET;

	inet_sock = socket(inet_domain, SOCK_DGRAM, 0);
	if(inet_sock == -1) error_exit("Inet socket initialization failed");

	inet_sockaddr.sin_family = inet_domain;
	inet_sockaddr.sin_port = htons(port_no);
	inet_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int inet_bind = bind(inet_sock, (struct sockaddr*) &inet_sockaddr, sizeof(inet_sockaddr));
	if(inet_bind == -1) error_exit("Inet bind failed.");



}

void empty_client(int i){
	if(clients[i].nick != NULL) free(clients[i].nick);
	if(clients[i].addr != NULL) free(clients[i].addr);
	clients[i].nick = NULL;
	clients[i].addr = NULL;
	clients[i].fd = -1;
	clients[i].game = NULL;
	clients[i].active = 0;
	clients[i].symbol = '-';
	clients[i].opponent_idx = -1;
}

void process_move(game* game){
	static int wins[8][3] = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
							  {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
							  {0, 4, 8}, {2, 4, 6} };

	for(int i = 0; i < 8; i++){

		char winning_char = game->board[wins[i][0]];
		if( game->board[wins[i][1]] == winning_char
			&& game->board[wins[i][2]] == winning_char){

			game->winner = winning_char;
			return;
		}
	}

	int any_empty = 0;
	for(int i = 0; i < 9; i++){
		if(game->board[i] == '-'){
			any_empty = 1;
			break;
		}
	}

	if(any_empty == 0) game->winner = 'D';

	if(game->turn == 'X') game->turn = 'O';
	else if(game->turn == 'O') game->turn = 'X';

}

int is_nick_available(char* nick){
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(is_client(i) && strcmp(nick, clients[i].nick) == 0) return 0;
	}
	return 1;
}

void start_game(int id1, int id2){

	clients[id1].opponent_idx = id2;
	clients[id2].opponent_idx = id1;

	int beg = rand() % 2;
	clients[id1].symbol = (beg == 0) ? 'O' : 'X';
	clients[id2].symbol = (beg == 0) ? 'X' : 'O';

	//if message type is GAME_FOUND, then winner means only whose symbol it is

	game* game = malloc(sizeof(game));
	empty_game_board(game);

	clients[id1].game = clients[id2].game = game;

	game->winner = clients[id1].symbol;
	send_message_to(clients[id1].fd, clients[id1].addr, GAME_FOUND, game, clients[id1].nick);

	game->winner = clients[id2].symbol;
	send_message_to(clients[id2].fd, clients[id2].addr, GAME_FOUND, game, clients[id2].nick);

	game->winner = '-';


}

void delete_game(game* game){
	free(game);
}

void connect_client(int fd, struct sockaddr* addr, char* rec_nick){
	printf("Connecting client\n");



	char* nick = calloc(NICK_LEN, sizeof(char));

	strcpy(nick, rec_nick);

	if(is_nick_available(nick) == 0){
		send_message_to(fd, addr, CONNECT_FAILED, NULL, "Your nick is already taken");
		free(nick);
		return;
	}
	if(first_free == MAX_CLIENTS){
		send_message_to(fd, addr, CONNECT_FAILED, NULL, "Server is full");
		free(nick);
		return;
	}

	send_message_to(fd, addr, CONNECT, NULL, "Connected");


	clients[first_free].nick = nick;
	clients[first_free].active = 1;
	clients[first_free].fd = fd;
	clients[first_free].addr = addr;


	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].nick != NULL) printf("%d: %s\n",i, clients[i].nick);
	}

	if(waiting_idx != -1){
		start_game(first_free, waiting_idx);
		waiting_idx = -1;
	}
	else{
		waiting_idx = first_free;
		send_message_to(fd, clients[first_free].addr, WAIT, NULL, nick);
		printf("WAIT sent\n");
	}

	first_free++;

	printf("Connected\n");

}

void net_routine(void* arg){

	struct pollfd poll_fds[2];

	poll_fds[0].fd = local_sock;
	poll_fds[1].fd = inet_sock;
	poll_fds[0].events = POLLIN;
	poll_fds[1].events = POLLIN;

	for( ; ; ) {

		pthread_mutex_lock(&clients_mutex);
		printf("Locked mutex in net_routine1\n");

		for (int i = 0; i < 2; i++) {
			poll_fds[i].events = POLLIN;
			poll_fds[i].revents = 0;
		}

		pthread_mutex_unlock(&clients_mutex);
		printf("Unlocked mutex in net_routine1\n");

		printf("Polling...\n");
		if(poll(poll_fds, 2, -1) == -1) error_exit("Poll failed.");
		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < 2; i++){
			if(poll_fds[i].revents && POLLIN){
				struct sockaddr* addr = malloc(sizeof(struct sockaddr));
				socklen_t len = sizeof(&addr);
				printf("DESCRIPTOR: %d\n", poll_fds[i].fd);
				message msg = receive_message_from(poll_fds[i].fd, addr, len);
				printf("Message received in server\n");
				int j;
				switch (msg.message_type) {
					case CONNECT:
						connect_client(poll_fds[i].fd, addr, msg.nick);
						break;
					case MOVE:
						printf("Received move\n");
						j = get_client_index(msg.nick);
						process_move(&msg.game);
						if(msg.game.winner == '-') {
							send_message_to(clients[clients[j].opponent_idx].fd, clients[clients[j].opponent_idx].addr, MOVE, &msg.game, clients[clients[j].opponent_idx].nick);
						}

						else{
							send_message_to(clients[j].fd, clients[j].addr, GAME_FINISHED, &msg.game, clients[j].nick);
							send_message_to(clients[clients[j].opponent_idx].fd, clients[clients[j].opponent_idx].addr, GAME_FINISHED, &msg.game, clients[clients[j].opponent_idx].nick);
							delete_game(clients[j].game);
						}
						free(addr);
						break;
					case PING:
						j = get_client_index(msg.nick);
						clients[j].active = 1;
						free(addr);
						break;
					case DISCONNECT:
						j = get_client_index(msg.nick);
						printf("Received disconnect from client\n");
						empty_client(j);
						free(addr);
						break;
					default:
						free(addr);
						break;
					}
				}
		}

		pthread_mutex_unlock(&clients_mutex);
		printf("Locked mutex in net_routine2\n");
	}

}



void ping_routine(void* arg){
	for( ; ; ){
		sleep(PING_INTERVAL);
		printf("Ping in progress...\n");

		pthread_mutex_lock(&clients_mutex);
		printf("Locked mutex in ping_routine\n");


		for(int i = 0; i < MAX_CLIENTS; i++){
			if(is_client(i)){
				clients[i].active = 0;
//				printf("PING sent to %s\n", clients[i].nick);
				send_message_to(clients[i].fd, clients[i].addr, PING, NULL, clients[i].nick);
			}
		}

		pthread_mutex_unlock(&clients_mutex);

		sleep(PING_WAIT);

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){
			if(is_client(i) && clients[i].active == 0) {
				printf("Response from %d was not received. Disconnecting %d...\n", i, i);
				send_message_to(clients[i].fd, clients[i].addr, DISCONNECT, NULL, clients[i].nick);
				empty_client(i);
			}
		}

		pthread_mutex_unlock(&clients_mutex);
		printf("Unlocked mutex in ping_routine\n");

		printf("Ping ended.\n");

	}

}


int main(int argc, char** argv){

	if(argc < 3) error_exit("Invalid arguments. Expected: port_number socket_path");

	port_no = atoi(argv[1]);
	socket_path = argv[2];

	signal(SIGINT, sigint_handler_server);

	for(int i = 0; i < MAX_CLIENTS; i++){
		empty_client(i);
	}

	start_local();
	start_inet();
	printf("Started...\n");
	waiting_idx = -1;
	first_free = 0;

	if(pthread_create(&net_thread, NULL, (void*) net_routine, NULL) == -1) error_exit("Could not create net thread.");
	if(pthread_create(&ping_thread, NULL, (void*) ping_routine, NULL) == -1) error_exit("Could not create ping thread");


	if(pthread_join(net_thread, NULL) < 0) error_exit("Could not join net thread.");
	if(pthread_join(ping_thread, NULL) < 0) error_exit("Could not join ping thread.");

	close_server();

	exit(EXIT_SUCCESS);

}