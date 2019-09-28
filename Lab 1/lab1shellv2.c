#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	
	// Declare Variables
	char inputBuffer[100]; // keyboard input
	char *token; // single token
	char *tokenArray[100]; // all tokens
	int tokenCount; // total number of tokens
	int pid; // holds return value of fork (PID of child)
	int i; // used for loop (printing tokens)
	
	printf("\n***Welcome to the Lab 1 Shell!***\n");
	
	while (!feof(stdin)){ // check for EOF (end of file) from keyboard
		
		printf("\nPlease type the directory and program/command, "
		   "along with the arguments you would like to execute.\n"
		   "End the program by pressing EOF (CTRL + D)\n"
		   "(e.g. /bin/ls -l -a)\n");
		
		// read input
		while(gets(inputBuffer)){ // check that an input has been received
		
			// fork 
			pid = fork(); 
			
			if (pid != 0) {
			/* PARENT process executes here */	
			
				wait(NULL); //WAIT FOR CHILD
				
			} else {
			/* CHILD process executes here */
				
				// tokenize input
				tokenCount = 0;
				token = strtok(inputBuffer, " "); 
				while(token != NULL){ 
					tokenArray[tokenCount] = token;
					tokenCount++;
					token = strtok(NULL, " ");
				}
				
				// print tokens
	//			printf("Tokens are:\n\n"); 
	//			for(i = 0; i < tokenCount; i++){
	//				printf("%s\n", tokenArray[i]);
	//			}
	//			printf("\n\n");
				
				// append NULL TERMINATOR to end of array
				tokenArray[tokenCount] = '\0'; 
				
				//exec command 
				execv(tokenArray[0], tokenArray); // EXECUTE COMMAND AND PASS ARGUMENTS
				
				//if exec returns, it must have failed
				printf("Error - unknown program.\n");
				
				exit(0);
			} /* End of If-Else */
			
			printf("\nPlease type the directory and program/command, "
		   "along with the arguments you would like to execute.\n"
		   "End the program by pressing EOF (CTRL + D)\n"
		   "(e.g. /bin/ls -l -a)\n");
		   
		} /* End of GETS() While */
	} /* End of EOF While */
	
	printf("\nProgram ended.\n");
	
}/* End Main */
			