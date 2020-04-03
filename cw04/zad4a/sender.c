#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int received = 0;
int catching = 0;
int sig_1 = SIGUSR1;
int sig_2 = SIGUSR2;
int is_sigqueue = 0;
int last_received;
int counter;

void sigusr_action(int sig, siginfo_t* info, void* ucontext){
	if(is_sigqueue) last_received = info->si_value.sival_int;
	if(sig == sig_1){
		received++;
	}
	else if(sig == sig_2){

		printf("Received %d signals in sender, expected %d signals\n", received, counter);
		if(is_sigqueue) printf("Last received signal: %d\n", last_received);
		exit(EXIT_SUCCESS);
	}
}


int main(int argc, char** argv){

	if(argc<4 || (strcmp(argv[3], "kill") != 0 && strcmp(argv[3], "sigqueue") != 0 && strcmp(argv[3], "sigrt") != 0 )){
		printf("Invalid arguments. Expected: catcher_pid sig_count kill/sigqueue/sigrt");
		exit(EXIT_FAILURE);
	}


	pid_t catcher_pid = atoi(argv[1]);

	counter = atoi(argv[2]);

	if(catcher_pid == 0 || counter == 0){
		printf("Invalid arguments. Expected: catcher_pid sig_count kill/sigqueue/sigrt");
		exit(EXIT_FAILURE);
	}

	if(strcmp(argv[3], "sigrt") == 0){
		sig_1 = SIGRTMIN;
		sig_2 = SIGRTMIN+1;
	}

	struct sigaction action;

	action.sa_flags = SA_SIGINFO;

	sigfillset(&action.sa_mask);
	sigdelset(&action.sa_mask, sig_1);
	sigdelset(&action.sa_mask, sig_2);

	action.sa_sigaction = sigusr_action;

	sigaction(sig_1, &action, NULL);
	sigaction(sig_2, &action, NULL);



	if(strcmp(argv[3], "sigqueue") == 0){
		is_sigqueue = 1;
		union sigval sigval;
		for(int i = 0; i<counter; i++){
			sigval.sival_int = i;
			sigqueue(catcher_pid, sig_1, sigval);

		}
		sigval.sival_int = counter;
		sigqueue(catcher_pid, sig_2, sigval);
	}
	else{
		for(int i = 0; i<counter; i++){
			kill(catcher_pid, sig_1);
		}
		kill(catcher_pid, sig_2);
	}

	while(1){
		pause();
	}


}