#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main (int argc, char *argv[]) {

	// DECLARATIONS
	int pidToKill;
	char correctInput;
	
	// Initialize
	correctInput = 0;
	
	// check for correct args
	if (argv[1] != NULL){
		
		// retrieve arguments
		pidToKill = atoi(argv[1]);
		
		correctInput = 1;
	
	}else{
		
		printf("Incorrect amount of arguments arguments.\n");
		
	}/* End of If-Else */

	// check for correct input
	if(correctInput){
	
		kill(pidToKill, SIGUSR1); //send SIGUSR1 signal to pidToKill
		printf("Sending kill signal to process with id: %d\n", pidToKill);
	
	} /* End of If */
		
} /* End of Main */