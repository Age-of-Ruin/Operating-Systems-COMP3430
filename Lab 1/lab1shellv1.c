#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	char inputBuffer[100];
	char *token;
	char *tokenArray[100];
	int tokenCount;
	int returnCode;
	int pid;
	char *command;
	
	printf("\n***Welcome to the Lab 1 Shell!***\n");
	
	printf("Please type the directory and program/command "
		   "you would like to execute.\n(e.g. /bin/date)\n");

	gets(inputBuffer); // read input
	
	token = strtok(inputBuffer, " "); // tokenize input
	while(token != NULL){ 
			tokenArray[tokenCount] = token;
			tokenCount++;
			token = strtok(NULL, " ");
	}
	
//	printf("Tokens are:\n"); // print tokens
//	for(int i = 0; i < tokenCount; i++){
//		printf("%s\n", tokenArray[i]);
//	}
				
	printf("Running: %s\n", tokenArray[0]);
	
	while (!feof(stdin)){ // check for EOF (end of file)
		
		pid = fork(); // fork 
		
		if (pid!=0) {
            /* parent process executes here */	

			sleep(1);
			
        } else {
            /* child process executes here */
			
			command = tokenArray[0];
			returnCode = execl(command, NULL); //exec command (done by child)
			
			if(returnCode != 0)
				printf("Error - unknown program.\n");
			
            exit(0);
        } /* end if-then-else */

		
		printf("Please type the directory and program/command "
			   "you would like to execute.\n");
		
		gets(inputBuffer); // read input
		
		token = strtok(inputBuffer, " "); // tokenize input
		while(token != NULL){ 
				tokenArray[tokenCount] = token;
				tokenCount++;
				token = strtok(NULL, " ");
		}
		
		printf("Running: %s\n", tokenArray[0]);
	
	} /* End of While */

	printf("\nProgram ended.\n");

} /* end main */

