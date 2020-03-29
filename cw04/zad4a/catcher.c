#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int counter = 0;
int catching = 1;
pid_t sender_pid;


void sigusr1_action(int sig, siginfo_t* info, void* ucontext){
	counter++;
}

void sigusr2_action(int sig, siginfo_t* info, void* ucontext){
	catching = 0;
	sender_pid = info->si_pid;
}

int main(int argc, char** argv){


	printf("Catcher PID %d\n",getpid());

	struct sigaction action;
	action.sa_flags = SA_SIGINFO;
	sigfillset(&action.sa_mask);
	sigdelset(&action.sa_mask, SIGUSR1);
	sigdelset(&action.sa_mask, SIGUSR2);

	sigaction()
	while(catching){
		usleep(100);
	}





}