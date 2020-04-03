#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int is_waiting = 0;

void sigtstp_handler(int sig_no){
	if (is_waiting == 0) printf("\nWaiting for CTRL+Z - continue or CTRL+C - end program\n");
	is_waiting = (is_waiting+1)%2;
	return;
}

void sigint_handler(int sing_no){
	printf("\nReceived signal SIGINT\n");
	exit(EXIT_SUCCESS);
}

int main(int arg, char** argv){

	struct sigaction sigtstp_action;
	sigtstp_action.sa_handler = sigtstp_handler;
	sigemptyset(&sigtstp_action.sa_mask);
	sigtstp_action.sa_flags = 0;
	sigaction(SIGTSTP, &sigtstp_action, NULL);

	signal(SIGINT, sigint_handler);
	while(1){
		if(is_waiting == 0) system("ls");
		sleep(1);
}


	return 0;
}