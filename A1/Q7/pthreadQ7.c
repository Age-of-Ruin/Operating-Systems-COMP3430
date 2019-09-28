#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global variable for array
int *ar;

// structure for multiple argument
struct argStruct {
	int threadNumElts;
	int start;
};


void *findMin(void *argsIn)
{
	struct argStruct *args = (struct argStruct *) argsIn;
	
	// get values from arg structure
	int threadNumElts = args->threadNumElts;
	int start = args->start;
	
	//printf("Starting index: %d\n", start);
	
	// find min
	int i; // FOR loop index
	int min; 
	
	min = ar[start];
	for(i = start; i < (start + threadNumElts); i++){
		if(ar[i] < min){
			min = ar[i];
		}
	}	
	
	// 'return' minimum found
	pthread_exit((void *)min);
	
} /* End of FindMin */


int main (int argc, char *argv[]) {

	// DECLARATIONS
	char correctInput; // used to check if arguments are correct
	char correctFile; // used to check if file are correct
	int numThreads; // tracks number of threads 
	int numElts; // tracks number of elements to worked on
	int numberCount; // used to count the numbers (for while loop)
	char tempInput[100]; // temporary string to hold fgets output
	FILE *numbers; // hold the file containing integers to be read
	int createCode; // holds the return code from pthread_create
	int joinCode; // hold the return code for pthread_join
	int *returnVal; // holds the return value of the thread
	int threadCount; // FOR loop index for creating threads
	int minVal; // stores the minimum value
	
	
	// INIT	
	correctInput = 0;
	correctFile = 0;
	numberCount = 0;
	
	// open file
	if(numbers = fopen("./numbers.txt", "r")){
		
		correctFile = 1;
		
	}else{
		
		printf("Cannot open file.\n");
		
	}
	
	// check for correct args
	if (argv[1] != NULL && argv[2] != NULL){
		
		// retrieve arguments
		numThreads = atoi(argv[1]);
		numElts = atoi(argv[2]);
		
		// establish array and amount of threads
		ar = (int *)malloc(numElts * sizeof(int));
		
		// establish correct input
		correctInput = 1;
		
	}else{
		
		printf("Incorrect amount of arguments arguments.\n");
	
	}/*End of If-Else */
		
	// check for correct input
	if (correctInput && correctFile){
		
		// create struct to pass to thread
		struct argStruct *args = malloc(sizeof(struct argStruct)); 
		args->threadNumElts = numElts/numThreads;
		args->start = 0;
		
		// read input
		while(fgets(tempInput, 100, numbers) && numberCount < numElts){			
		
			ar[numberCount] = atoi(tempInput);
		
			numberCount++;

		} /* End of While */
		
		// create threads & find minimum
		minVal = ar[args->start]; // set initial min value to starting value
		pthread_t threads[numThreads]; // establish array of thread ID's
		for(threadCount = 0; threadCount < numThreads; threadCount++){
			
			// create thread
			createCode = pthread_create(&threads[threadCount], NULL, findMin, args);
	
			// wait for threads to find each min
			joinCode = pthread_join(threads[threadCount], (void **)&returnVal);
			
			// increment start
			args->start += args->threadNumElts;
			
			// store minimum value found by each thread
			if((int) returnVal < minVal)
				minVal = (int) returnVal;					
				
		}/*End of For */
		
		printf("The minimum value found: %d", minVal);
		
		printf("\nProgram Ends\n");
		
		pthread_exit(NULL);	// this line is important!
		
	} /*End of If */
} // End main

