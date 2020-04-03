#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char** argv){
	if(argc<2){
		printf("Invalid arguments\n");
		exit(EXIT_FAILURE);
	}

	raise(SIGUSR1);

	if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
		sigset_t mask;
		sigpending(&mask);
		if(sigismember(&mask, SIGUSR1)){
			printf("Pending in child: %d\n", SIGUSR1);
		}
		else{
			printf("Not pending in child\n");
		}
	}


	return 0;

}