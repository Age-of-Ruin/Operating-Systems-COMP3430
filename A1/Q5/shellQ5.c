#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>

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
	char *saveptr; // used for str_tok to save place
	
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
	
	int i,j;
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
				tokenArray[i] = swapVarsWithinToken(posnCount, tokenArray[i], newToken, varPosn, inputVars, outputVars, varCount);	
						
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
	if (file = fopen("./.shell_init", "r")){
		
		//read all lines of file and set variables 
		while(fgets(lineBuffer, 100, file)){
			
			// tokenize the line
			tokenCount = tokenize(lineBuffer, lineTokenized, " ", saveptr);
			
			// Remove 'return' and newline (erroneus) input characters by appending null terminator
			lineTokenized[1][strcspn(lineTokenized[1], "\r\n")] = '\0';
			
			// set variable if valid
			if(setVar(lineTokenized, inputVars, outputVars, varCount)){
				varCount++;
			}/*End of IF */ 
		}/*End of While */ 
	}/*End of IF */ 
	
	return varCount;

}/* End of presetVarsFromFile*/


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
	
/********************** PRESET VARIABLES (if required) - Q5 ***************************/
	// preset vars from file and update varCount
	varCount = presetVarsFromFile(inputVars, outputVars, varCount);
	
	printf("\n***Welcome to the A1 Q5 Shell!***\n");
	
    printf("\n->");
		
	/********************** Check and Set variable (if required) - Q4 ***************************/
		
		// check that an input has been received & check EOF
		while(gets(inputBuffer)){ 
			
			// check for empty input
			if(inputBuffer[0] != '\0'){					
			
				// tokenize input
				tokenCount = tokenize(inputBuffer, tokenArray, " ", saveptr); 
				 
				// check for valid 'set' command and set variable
				setCMD = setVar(tokenArray, inputVars, outputVars, varCount);
				if (setCMD){ 
						
					// increment variable count
					varCount++;
						
				}else{
				
				/********************** Check and Replace variable (if required) - Q4 ***************************/			
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
	
						//exec command 
						execvp(tokenArray[0], tokenArray); // EXECUTE COMMAND AND PASS ARGUMENTS
						
						//if exec returns, it must have failed
						printf("\nError - unknown program.\n");	
						
					} /* End of If */
					
					// exit child process
					exit(0);
					
				} /* End of If-Else */
			}/* End of If */
		
		printf("\n->");

		} /* End of gets() While */
	
	printf("\nProgram ended.\n");
	
}/* End Main */
			