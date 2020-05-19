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
	return i>= 0 && i < MAX_CLIENTS && clients[i].fd != -1;
}

void close_server(){

	if(pthread_cancel(net_thread) == -1) error_exit("Could not cancel net tread");
	if(pthread_cancel(ping_thread) == -1) error_exit("Could not cancel ping thread");
	close(local_sock);
	unlink(socket_path);
	close(inet_sock);
}


void start_local(){
	int local_domain = AF_UNIX;

	local_sock = socket(local_domain, SOCK_STREAM, 0);
	if(local_sock == -1) error_exit("Local socket initialization failed.");

	local_sockaddr.sa_family = local_domain;
	strcpy(local_sockaddr.sa_data, socket_path);

	int local_bind = bind(local_sock, &local_sockaddr, sizeof(local_sockaddr));
	if(local_bind == -1) error_exit("Local bind failed.");

	int local_listen = listen(local_sock, MAX_CLIENTS);
	if(local_listen == -1) error_exit("Local listen failed.");

	printf("Local socket fd: %d\n", local_sock);

}

void start_inet(){
	int inet_domain = AF_INET;

	inet_sock = socket(inet_domain, SOCK_STREAM, 0);
	if(inet_sock == -1) error_exit("Inet socket initialization failed");

	inet_sockaddr.sin_family = inet_domain;
	inet_sockaddr.sin_port = htons(port_no);
	inet_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int inet_bind = bind(inet_sock, (struct sockaddr*) &inet_sockaddr, sizeof(inet_sockaddr));
	if(inet_bind == -1) error_exit("Inet bind failed.");

	int inet_listen = listen(inet_sock, MAX_CLIENTS);
	if(inet_listen == -1) error_exit("Inet listen failed.");

}

void disconnect_client(int i){
	printf("Disconnecting %s\n", clients[i].nick);
	if(!is_client(i)) return;
	if(shutdown(clients[i].fd, SHUT_RDWR) < 0)
		error_exit("Could not disconnect client.");

	if(close(clients[i].fd) < 0)
		error_exit("Could not close client.");

}


void empty_client(int i){
	if(clients[i].nick != NULL) free(clients[i].nick);
	clients[i].nick = NULL;
	clients[i].fd = -1;
	clients[i].game = NULL;
	clients[i].active = 0;
	clients[i].symbol = '-';
	clients[i].opponent_idx = -1;
	if(waiting_idx == i) waiting_idx = -1;
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

int get_free_idx(){
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(!is_client(i)) return i;
	}
	return -1;
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

	game* new_game = malloc(sizeof(game));
	empty_game_board(new_game);

	clients[id1].game = clients[id2].game = new_game;

	new_game->winner = clients[id1].symbol;
	send_message(clients[id1].fd, GAME_FOUND, new_game, NULL);

	new_game->winner = clients[id2].symbol;
	send_message(clients[id2].fd, GAME_FOUND, new_game, NULL);

	new_game->winner = '-';

}

void delete_game(game* game){
	free(game);
}

void connect_client(int fd){
	printf("Connecting client\n");

	int client_fd = accept(fd, NULL, NULL);
	if(client_fd < 0) error_exit("Could not accept client.");

	message msg = receive_message(client_fd);

	char* nick = calloc(NICK_LEN, sizeof(char));

	strcpy(nick, msg.nick);

	if(is_nick_available(nick) == 0){
		send_message(client_fd, CONNECT_FAILED, NULL, "Your nick is already taken");
		return;
	}
	if(first_free == -1){
		send_message(client_fd, CONNECT_FAILED, NULL, "Server is full");
	}

	send_message(client_fd, CONNECT, NULL, "Connected");


	clients[first_free].nick = nick;
	clients[first_free].active = 1;
	clients[first_free].fd = client_fd;


	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].nick != NULL) printf("%d: %s\n",i, clients[i].nick);
	}

	if(waiting_idx != -1){
		start_game(first_free, waiting_idx);
		waiting_idx = -1;
	}
	else{
		waiting_idx = first_free;
		send_message(client_fd, WAIT, NULL, NULL);
		printf("WAIT sent\n");
	}

	first_free = get_free_idx();

	printf("Connected\n");

}

void net_routine(void* arg){

	struct pollfd poll_fds[MAX_CLIENTS + 2];

	poll_fds[MAX_CLIENTS].fd = local_sock;
	poll_fds[MAX_CLIENTS + 1].fd = inet_sock;


	for( ; ; ) {

		pthread_mutex_lock(&clients_mutex);

		for (int i = 0; i < MAX_CLIENTS + 2; i++) {
			if(i < MAX_CLIENTS) poll_fds[i].fd = clients[i].fd;
			poll_fds[i].events = POLLIN;
			poll_fds[i].revents = 0;
		}

		pthread_mutex_unlock(&clients_mutex);

		printf("Polling...\n");
		if(poll(poll_fds, MAX_CLIENTS + 2, -1) == -1) error_exit("Poll failed.");

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS + 2; i++){
			if(i < MAX_CLIENTS && !is_client(i)) continue;

			if(poll_fds[i].revents && POLLIN){
				if(poll_fds[i].fd == local_sock || poll_fds[i].fd == inet_sock){
					connect_client(poll_fds[i].fd);
				}
				else{
					message msg = receive_message(poll_fds[i].fd);
//					printf("Message received in server\n");
					switch (msg.message_type) {
						case MOVE:
						printf("Received move\n");
						process_move(&msg.game);
						if(msg.game.winner == '-') {
							send_message(clients[clients[i].opponent_idx].fd, MOVE, &msg.game, NULL);
						}

						else{
							send_message(poll_fds[i].fd, GAME_FINISHED, &msg.game, NULL);
							send_message(clients[clients[i].opponent_idx].fd, GAME_FINISHED, &msg.game, NULL);
							delete_game(clients[i].game);
						}
						break;
					case PING:
						clients[i].active = 1;
						break;
					case DISCONNECT:
						printf("Received disconnect from client\n");
						if(is_client(clients[i].opponent_idx)){
							disconnect_client(clients[i].opponent_idx);
							empty_client(clients[i].opponent_idx);
						}
						disconnect_client(i);
						empty_client(i);
						break;
					default:
						break;
					}
				}
			}else if(is_client(i) && poll_fds[i].revents && POLLHUP){
				printf("Disconnect...\n");
				disconnect_client(i);
				empty_client(i);
			}
		}

		pthread_mutex_unlock(&clients_mutex);

	}

}



void ping_routine(void* arg){
	for( ; ; ){
		sleep(PING_INTERVAL);
		printf("Ping in progress...\n");

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){
			if(is_client(i)){
				clients[i].active = 0;
//				printf("PING sent to %s\n", clients[i].nick);
				send_message(clients[i].fd, PING, NULL, NULL);
			}
		}

		pthread_mutex_unlock(&clients_mutex);

		sleep(PING_WAIT);

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){
			if(is_client(i) && clients[i].active == 0) {
				printf("Response from %d was not received. Disconnecting %d...\n", i, i);
				send_message(clients[i].fd, DISCONNECT, NULL, NULL);
				if(is_client(clients[i].opponent_idx)){
					disconnect_client(clients[i].opponent_idx);
					empty_client(clients[i].opponent_idx);
				}
				disconnect_client(i);
				empty_client(i);
			}
		}

		pthread_mutex_unlock(&clients_mutex);
		printf("Ping ended.\n");

	}

}

void at_exit_fun(){
	close_server();
}

void sigint_handler_server(int signo){
	exit(EXIT_SUCCESS);
}


int main(int argc, char** argv){

	if(argc < 3) error_exit("Invalid arguments. Expected: port_number socket_path");

	port_no = atoi(argv[1]);
	socket_path = argv[2];

	atexit(at_exit_fun);
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