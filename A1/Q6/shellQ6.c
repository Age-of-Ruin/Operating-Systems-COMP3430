#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int tokenize(char *inputBuffer, char **tokenArray, char delimit[], char *ptr){
	char *token; // single string token
	int tokenCount; // track number of tokens
	
		// tokenize input
		tokenCount = 0;
		token = strtok_r(inputBuffer, delimit, &ptr); 
		while(token != NULL){ 
			
			tokenArray[tokenCount] = token;
			tokenCount++;
			token = strtok_r(NULL, delimit, &ptr);
			
		}/*End of While */				
					
		// append NULL TERMINATOR to end of token array
		tokenArray[tokenCount] = '\0'; 
		
		return tokenCount;

} /* End of tokenize*/


void printTokens(char **tokenArray, int tokenCount){	
	int i;
	
	printf("\nTokens are:\n\n"); 
	for(i = 0; i < tokenCount; i++){
		printf("%s\n", tokenArray[i]);
	}
	
	printf("\n\n");
	
} /* End of printTokens*/


char setVar(char **tokenArray, char **inputVars, char **outputVars, int varCount){
	
	char *vars[2]; // input and output variables
	char success = 0; // test if variable was set
	char *saveptr;
	
	// check for '=' and set command
	if(tokenArray[1] != NULL){
		if(!strcmp(tokenArray[0], "set") && strstr(tokenArray[1], "$") && strstr(tokenArray[1], "=")){
			
			// tokenize variables
			tokenize(tokenArray[1], vars, "=", saveptr);

			// and check that input and output variables are valid
			if(*vars[0] != '\0' && *vars[1] != '\0'){
				
				// save variables
				inputVars[varCount] = strdup(vars[0]);
				outputVars[varCount] = strdup(vars[1]);
				
				// valid variable was entered
				success = 1;
						
			}/* End of IF */
		}/* End of IF */
	}/* End of IF */
	
	return success;

}/* End of setVar*/

char *swapVarsWithinToken(int posnCount, char *token, char *newToken, int varPosn[], char **inputVars, char **outputVars, int varCount){

	int i,j; // used in FOR loops
	
	char tempVar[100]; // used to store the potential variable to be checked
	int tempCount; // used as a counter for the temp variable
	
	char *swappedVars[100]; // hold any output variables to be used
	int validVarCount; // holds a count of valid variables
	int validVarPosn[100]; // holds the postion of variables needing to be replaced
	
	char success = 0; // indicator for successful variable substitution
	
	int inputLengths[100]; // used to store input lengths of input variables (for token reconstruction) 
	
	int totalTokenLength = strlen(token); // holds the total resultant token length
	
	char resultString[totalTokenLength]; // holds resultant token
	
	
	// Initialize valid vars to 0
	validVarCount = 0;
	validVarPosn[0] = 0;
	
	// loop over potential variables
	for(i = 0; i < posnCount; i++){

		// loop over characters of each variable within token and extract them into temp variable
		tempCount = 0;
		tempVar[tempCount] = token[varPosn[i]];		
		tempCount++;
		for(j = varPosn[i]+1 ; isalpha(token[j]); j++){
			tempVar[tempCount] = token[j]; 
			tempCount++;	
		}
		tempVar[tempCount] = '\0';
		
		//printf("Input Var: %s\n", tempVar); 
		
		// check for variables within inputVar table and store it if exists
		for(j = 0; j < varCount; j++){
			
			// make sure there are variables stored
			if(inputVars[j] != NULL && outputVars[j] != NULL){
				
				// compare the current variable to list of stored variables
				if(!strcmp(tempVar, inputVars[j])){
					
					// store the appropriate output
					swappedVars[validVarCount] = outputVars[j];
					
					//printf("Swapped Var: %s\n", swappedVars[validVarCount]);
					
					// store length of input variable
					inputLengths[validVarCount] = strlen(inputVars[j]);
					
					// store relevant variable information
					validVarPosn[validVarCount] = varPosn[i];
					validVarCount++;
					success = 1;
				}/* End of IF */
			}/* End of IF */
		}/* End of FOR */
	}/* End of FOR */
	
/********************** Reconstruct Token **************************/		
	// calculate total length of resultant token
	for (i = 0; i < validVarCount; i ++)
		totalTokenLength += strlen(swappedVars[i]);

	// reconstruct the token and substitute variables
	if(success){
	strncpy(resultString, token, validVarPosn[0]);
		for(i = 0; i < validVarCount; i++){
			resultString[validVarPosn[i]-i] = '\0';
			strcat(resultString, swappedVars[i]);
			strcat(resultString, token + validVarPosn[i] + inputLengths[i]);
		}/*End of FOR */
	}/*End of IF */
	
	// if successful swap return new token
	if (success){

		return strdup(resultString);
	
	// else return old token (no change occured)
	}else
		
		return token;	
	/* End of IF-ELSE */
}/* End of swapVarsWithinToken*/

void checkAndSwapVars(char **tokenArray, int tokenCount, char **inputVars, char **outputVars, int varCount){
	
	int i,j,k;
	int varPosn[100];
	int posnCount;
	char newToken[1000];

	// check if variable to be converted was entered
		for (i = 0; i < tokenCount; i++){
			
			posnCount = 0;
			
			// check if variable substitution is required within input token by checking for '$'			
			if(strstr(tokenArray[i], "$")){
			
				// loop to find '$' within token and store their position
				for(j = 0; j < strlen(tokenArray[i]); j++){
					if(tokenArray[i][j] == '$'){
						varPosn[posnCount] = j;
						posnCount++;		
					}/*End of IF */
				}/*End of FOR */
				
				// check if variables exist and swap
				//("Altering token %d\n", i);
				//printf("token %d: %s\n", i, tokenArray[i]);
				tokenArray[i] = swapVarsWithinToken(posnCount, tokenArray[i], newToken, varPosn, inputVars, outputVars, varCount);
				//printf("token %d: %s\n\n", i, tokenArray[i]);			
						
			}/*End of IF */ 
		}/*End of FOR */	
	
}/* End of checkAndSwapVars*/


int presetVarsFromFile(char **inputVars, char **outputVars, int varCount){
	
	FILE *file;
	char lineBuffer[100];
	char *lineTokenized[100];
	char *saveptr;
	int tokenCount;
	
	// open file
	if (file = fopen(".shell_init", "r")){
		
		//read all lines of file and set variables 
		while(fgets(lineBuffer, 100, file)){
			
			// tokenize the line
			tokenCount = tokenize(lineBuffer, lineTokenized, " ", saveptr);
			
			// Remove 'return' and newline (erroneus) input characters by appending null terminator
			lineTokenized[1][strcspn(lineTokenized[1], "\r\n")] = '\0';
			
			// set variable if valid
			if(setVar(lineTokenized, inputVars, outputVars, varCount)){
				varCount++;
			}
		}
	}
	
	return varCount;

}/* End of presetVarsFromFile*/

int checkForOutputRedirect(char **tokenArray, int tokenCount){
	
	int i; // FOR loop
	int redirectPosn = -1; // used to track location of the redirect token
	
	for(i = 0; i < tokenCount; i++){
		
		// check for '>', '>>, '2>' and '2>>'
		if((!strcmp(tokenArray[i], ">") || 
			!strcmp(tokenArray[i], ">>") || 
			!strcmp(tokenArray[i], "2>") || 
			!strcmp(tokenArray[i], "2>>")) && 
			(tokenArray[i-1] != NULL && 
			tokenArray[i+1] != NULL)){
			
			redirectPosn = i;
			
		}/*End of IF */
	
	}/*End of FOR */
	
	return redirectPosn;
	
}/* End of checkForOutputRedirect*/

int checkForPipe(char **tokenArray, int tokenCount){
	
	int i; // FOR loop
	int pipePosn = -1; // used to track location of the redirect token
	
	for(i = 0; i < tokenCount; i++){
		
		// check for '>'
		if(!strcmp(tokenArray[i], "|") && tokenArray[i-1] != NULL && tokenArray[i+1] != NULL){
			
			pipePosn = i;
			
		}/*End of IF */
	
	}/*End of FOR */
	
	return pipePosn;
	
}/* End of checkForPipe*/

void performOutputRedirect(char **tokenArray, int redirectPosn){
	
	int closeError;
	int fd;
	char *cmdArgs[100];
	char *outputArg;
	int argCount;
	int i;
	int stdin_copy;
	int stdout_copy;
	int stderr_copy;
	char outputCMD;
	char errorCMD;
	
	// INIT
	outputCMD = 0;
	errorCMD = 0;
	
	// extract appropriate arguments
	argCount = 0;
	while(argCount < redirectPosn){
		cmdArgs[argCount] = tokenArray[argCount];
		argCount++;
	}
	cmdArgs[argCount] = '\0';
	
	// save output
	outputArg = tokenArray[argCount+1];
	
	// perform redirect
	// check for '>' (overwrite)
	if(tokenArray[argCount][0] == '>' && tokenArray[argCount][1] == '\0'){
		
		stdout_copy = dup(1);
		closeError = close(STDOUT_FILENO);
		fd = open(outputArg, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		outputCMD = 1;
	
	// check for '>>' (append)
	}else if (tokenArray[argCount][0] == '>' && tokenArray[argCount][1] == '>'){
		
		stdout_copy = dup(1);
		closeError = close(STDOUT_FILENO);
		fd = open(outputArg, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);
		outputCMD = 1;
		
	// check for '2>' (overwrite)
	}else if (tokenArray[argCount][0] == '2' && tokenArray[argCount][1] == '>'){
		
		stderr_copy = dup(2);
		closeError = close(STDERR_FILENO);
		fd = open(outputArg, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		errorCMD = 1;
		
	// check for '2>>'	(append)
	}else if (tokenArray[argCount][0] == '2' && tokenArray[argCount][1] == '>' && tokenArray[argCount][2] == '>'){
		
		stderr_copy = dup(2);
		closeError = close(STDERR_FILENO);
		fd = open(outputArg, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);	
		errorCMD = 1;
	}
	
	// EXECUTE CMD
	execvp(cmdArgs[0], cmdArgs); 
	
	// close old fd
	close(fd);
	
	//if exec returns, it must have failed
	if (outputCMD){
		// Reopen stdout
		dup2(stdout_copy,1);
		close(stdout_copy);
	}
	if (errorCMD){
		// Reopen stderr
		dup2(stderr_copy,2);
		close(stderr_copy);
	}
	
	printf("\nError - unknown program.\n");	
	
}/* End of performOutputRedirect*/

void performPipe(char **tokenArray, int tokenCount, int pipePosn){
	
	int pipeFDs[2]; // Hold pipe ends (file descriptors)
	int pid; // holds return value of fork (PID of child)
	char *argsBeforePipe[100];
	char *argsAfterPipe[100];
	int argCount;
	
	// INIT
	argCount = 0;
	
	// Construct pipe
	pipe(pipeFDs);
	
	// extract arguments
	while(argCount < tokenCount){
		
		if(argCount < pipePosn){
			argsBeforePipe[argCount] = tokenArray[argCount];
		}
		if(argCount > pipePosn){
			argsAfterPipe[argCount-pipePosn-1] = tokenArray[argCount];
		}
		
		argCount++;
	}
	argsBeforePipe[pipePosn] = '\0';
	argsAfterPipe[argCount-pipePosn-1] = '\0';
	
	// forka
	pid = fork();
	
	if(pid == 0){
		// Child Process
		dup2(pipeFDs[1], STDOUT_FILENO); // remap stdout to the 'write' end of the pipe (ie write back to parent)
		close(pipeFDs[0]); // close both child 'read' and 'write' ends of the pipe (using stdout)
		close(pipeFDs[1]);
		
		execvp(argsBeforePipe[0], argsBeforePipe); // execute CMD before pipe symbol (and pass to parent)

   } else {
       // Parent process
		dup2(pipeFDs[0],STDIN_FILENO); // remap stdin to the 'read' end of the pipe (ie read from child)
		close(pipeFDs[0]); // close both child 'read' and 'write' ends of the pipe (using stdout)
		close(pipeFDs[1]);
       
		execvp(argsAfterPipe[0], argsAfterPipe); // execute CMD after pipe symbol (and read from child)
   }
	
	printf("\nError - unknown program.\n");	

}/* End of performPipe*/


int main(int argc, char *argv[]) {
	
	// Declare Variables
	char inputBuffer[100]; // keyboard input
	char *tokenArray[100]; // all tokens
	int tokenCount; // number of tokens
	
	int pid; // holds return value of fork (PID of child)
	
	char *inputVars[100]; // holds user input variables
	char *outputVars[100]; // holds user output variables
	int varCount = 0; // tracks number of variables
	
	char setCMD; // check if set command was entered
	char validSetCMD; // check if valid set command
	char varSubNeeded; // check variable substitution is required
	
	int i; // FOR loop index
	
	char *saveptr; // used for str_tok to save place
	
	int redirectPosn; // used to hold redirect character (>) position within tokenArray
	int pipePosn; // used to hold pipe character (|) position within tokenArray
	
	
/********************** PRESET VARIABLES (if required) - Q5 ***************************/
	// preset vars from file and update varCount
	varCount = presetVarsFromFile(inputVars, outputVars, varCount);
	
	printf("\n***Welcome to the A1 Q6 Shell!***\n");
	
    printf("\n->");
		
	/********************** Check and Set variable (if required) - Q4 ***************************/
		
		// check that an input has been received & check EOF
		while(gets(inputBuffer)){ 
			
			// check for empty input
			if(inputBuffer[0] != '\0'){					
			
				// tokenize input using " "
				tokenCount = tokenize(inputBuffer, tokenArray, " ", saveptr); 
				 
				// check for valid 'set' command and set variable
				setCMD = setVar(tokenArray, inputVars, outputVars, varCount);
				if (setCMD){ 
						
					// increment variable count
					varCount++;
						
				}else{
				
		/********************** Check and Replace variable - Q4 ***************************/			
					
					checkAndSwapVars(tokenArray, tokenCount, inputVars, outputVars, varCount);
				
				}/* End of If-Else */
				
				// fork 
				pid = fork(); 
				
				/* PARENT process executes here - idle process keeps shell running */
				if (pid != 0) {
					
					//WAIT FOR CHILD
					wait(NULL); 
				
				/* CHILD process executes here - used to execute shell CMDs */	
				}else{
					if(!setCMD){
						
		/********************** Check for PIPE and REDIRECT - Q6 ***************************/
						
						// check for redirect
						redirectPosn = checkForOutputRedirect(tokenArray, tokenCount);
						if(redirectPosn != -1)
							performOutputRedirect(tokenArray, redirectPosn);							
						
						// check for pipe
						pipePosn = checkForPipe(tokenArray, tokenCount);
						if(pipePosn != -1)
							performPipe(tokenArray, tokenCount, pipePosn);
						
						
						if (redirectPosn == -1 && pipePosn == -1){
							//exec command 
							execvp(tokenArray[0], tokenArray); // EXECUTE COMMAND AND PASS ARGUMENTS
							
							//if exec returns, it must have failed
							printf("\nError - unknown program.\n");	
						}/* End of If */
					}/* End of If */
					
					// exit child process(es)
					exit(0);
					
				}/* End of If-Else */
			}/* End of If */
		
		printf("\n->");

		}/* End of While */
	
	printf("\nProgram ended.\n");
	
}/* End Main */
			