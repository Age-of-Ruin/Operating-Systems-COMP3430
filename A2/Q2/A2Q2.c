#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#define NUM_THREADS     2

pthread_t threads[NUM_THREADS];

void *threadOne()
{	
	printf("Thread 1 will read input from stdin and exit upon receiving input...\n");
	
	char buffer[100];
	fgets(buffer, 100, stdin);

	printf("You have entered: %s", buffer);
	printf("Thread 1 exiting...\n");
	
	pthread_exit(NULL);
} // end threadOne

void *threadTwo()
{
	printf("Thread 2 will sleep for 5 seconds then print 10 statements and exit...\n");
	sleep(5);
	
	int i;
	for (i = 0; i < 10; i++){
	   printf("Thread 2 Running...\n");
	} // End FOR
	
	printf("Thread 2 exiting...\n");
	   
	pthread_exit(NULL);
} // end threadTwo

int main (int argc, char *argv[]) {
  
	long returnCode, i;

	for(i = 0; i < NUM_THREADS; i++){

		if (i % 2 == 0) {
		 
			returnCode = pthread_create(&threads[i], NULL, 
			threadOne, NULL);				
			
		} else {
		 
			returnCode = pthread_create(&threads[i], NULL, 
			threadTwo, NULL);
				
		}// End If-Else

	} // End for	 
   
	pthread_exit(NULL);	
  
	return 0;
} // End main

