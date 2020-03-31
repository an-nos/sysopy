#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <stdint.h>

void action_queue(int sig, siginfo_t* info, void* ucontext){
	printf("Alarm\n");
	printf("Signal number %d\n", info->si_signo);
	printf("Sending PID %d\n", info->si_pid);
	printf("Received value %d\n", info->si_int);
	exit(EXIT_SUCCESS);

}

void action_child(int sig, siginfo_t* info, void* ucontext){
	printf("Child\n");
	printf("Signal number %d\n", info->si_signo);
	printf("Sending PID %d\n", info->si_pid);
	printf("Address of error %d\n", info->si_status);
	exit(EXIT_SUCCESS);
}

void action_segfault(int sig, siginfo_t* info, void* ucontext){
	printf("Segmentation fault\n");
	printf("Signal number %d\n", info->si_signo);
	printf("Sending PID %d\n", info->si_pid);
	printf("Address of error %p\n", info->si_addr);
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv){

	if(argc < 2){
		printf("Wrong arguments. Expected: queue/child/segfault\n");
		exit(EXIT_FAILURE);
	}

	struct sigaction s_action;
	s_action.sa_flags = SA_SIGINFO;
	sigemptyset(&s_action.sa_mask);

	if(strcmp(argv[1], "queue") == 0){
		s_action.sa_sigaction = action_queue;
		sigaction(SIGUSR1, &s_action, NULL);

		union sigval sigval;
		sigval.sival_int = 13;
		sigqueue(getpid(), SIGUSR1, sigval);

	}
	else if(strcmp(argv[1], "child") == 0){
		s_action.sa_sigaction = action_child;
		sigaction(SIGCHLD, &s_action, NULL);

		pid_t child_pid = fork();
		if(child_pid == 0){
			exit(42);
		}
		wait(0);
	}
	else if(strcmp(argv[1],"segfault") == 0){
		s_action.sa_sigaction = action_segfault;
		sigaction(SIGSEGV, &s_action, NULL);

		int* x = NULL;
		x[21] = 37;
	}
	else{
		printf("Invalid arguments. Expected: queue/child/segfault\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}