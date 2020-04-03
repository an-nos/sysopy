#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

void handler(int proc_no){
	printf("Process received\n");
}

int main(int argc, char** argv){

	if(argc<3 || (strcmp(argv[1], "handler") == 0 && strcmp(argv[2], "exec") == 0)){
		printf("Invalid arguments. Expected: ignore/handler/mask/pending fork/exec (do not combine handler exec)\n");
		exit(EXIT_FAILURE);
	}


	if(strcmp(argv[1], "ignore") == 0){
		signal(SIGUSR1, SIG_IGN);
	}
	else if (strcmp(argv[1], "handler") == 0){
		signal(SIGUSR1, handler);
	}
	else if( strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGUSR1);
		sigprocmask(SIG_BLOCK, &mask, NULL);
	}
	else{
		printf("Invalid arguments. Expected: ignore/handler/mask/pending fork/exec\n");
		exit(EXIT_FAILURE);
	}

	raise(SIGUSR1);

	if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
		sigset_t test_mask;
		sigpending(&test_mask);
		if(sigismember(&test_mask, SIGUSR1)){
			printf("Pending in parent: %d\n", SIGUSR1);
		}
	}


	if(strcmp(argv[2], "fork") == 0) {

		pid_t child_pid = fork();

		if (child_pid == 0) {

			if (strcmp(argv[1], "pending") != 0) {
				raise(SIGUSR1);
			}
			if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
				sigset_t test_mask;
				sigpending(&test_mask);
				if (sigismember(&test_mask, SIGUSR1)) {
					printf("Pending in child: %d\n", SIGUSR1);
				}
				else{
					printf("Not pending in child\n");
				}
			}
		}
	}
	else if(strcmp(argv[2], "exec") == 0 && strcmp(argv[1], "handler") != 0){
		execl("./exec_test", "./exec_test", argv[1], NULL);
	}
	else{
		printf("Invalid arguments. Expected: ignore/handler/mask/pending fork/exec\n");
		exit(EXIT_FAILURE);
	}


	wait(0);

	return 0;

}