#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int counter = 0;
int catching = 1;
pid_t sender_pid;
int sig_1 = SIGUSR1;
int sig_2 = SIGUSR2;
int is_sigqueue = 0;

void sigusr_action(int sig, siginfo_t* info, void* ucontext){
	if(sig == sig_1){
		counter++;
		sender_pid = info->si_pid;
	}
	else if(sig == sig_2){
		catching = 0;
	}

	if(is_sigqueue){
		union sigval sigval;
		sigval.sival_int = counter;
		sigqueue(sender_pid, sig, sigval);
	}
	else kill(sender_pid, sig);
}

int main(int argc, char** argv){

	if(argc < 2  || (strcmp(argv[1],"kill") !=0 && strcmp(argv[1], "sigqueue") != 0 && strcmp(argv[1], "sigrt") != 0)){
		printf("Invalid arguments. Expected: kill/sigqueue/sigrt\n");
		exit(EXIT_FAILURE);
	}


	if(strcmp(argv[1], "sigrt") == 0){
		sig_1 = SIGRTMIN;
		sig_2 = SIGRTMIN+1;
	}
	else if(strcmp(argv[1], "sigqueue") == 0){
		is_sigqueue = 1;
	}

	printf("Catcher PID %d\n",getpid());

	struct sigaction action;

	action.sa_flags = SA_SIGINFO;

	sigfillset(&action.sa_mask);
	sigdelset(&action.sa_mask, sig_1);
	sigdelset(&action.sa_mask, sig_2);


	action.sa_sigaction = sigusr_action;

	sigaction(sig_1, &action, NULL);
	sigaction(sig_2, &action, NULL);



	while(catching){
		usleep(1);
	}

	if(strcmp(argv[1], "sigqueue") == 0){
		union sigval sigval;
		for(int i = 0; i<counter; i++){
			sigval.sival_int = i;
			sigqueue(sender_pid, sig_1, sigval);
		}
		sigval.sival_int = counter;
		sigqueue(sender_pid, sig_2, sigval);
	}
	else{
		for(int i = 0; i<counter; i++){
			kill(sender_pid, sig_1);
		}
		kill(sender_pid, sig_2);
	}


	printf("Received %d signals in catcher\n",counter);

	exit(EXIT_SUCCESS);


}