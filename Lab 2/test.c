#include <stdio.h>
#include <pthread.h>
#include "list.h"

#define NUM_THREADS 6


void *threadFcn(void *myListPtr) {
	
	// Insert
	printf("Inserting Data: %ld\n", pthread_self()/1000);
	
	listInsert((listNodePtr *)&myListPtr, pthread_self(), pthread_self()/1000);

	// Search
	printf("Searching Data: %ld\n", listSearch(myListPtr, pthread_self()));
		
	// Delete
	listDelete((listNodePtr *)&myListPtr, pthread_self());
	
	printf("Does %ld still exist?: %ld\n", pthread_self()/1000, listSearch(myListPtr, pthread_self()));
		
	pthread_exit(NULL);
	
} // end threadFcn


	 	
int main(int argc, char *argv[]) {
	
	// declarations
	long i;
	pthread_t threads[NUM_THREADS];
	
	// create list
	listNodePtr myListPtr;
	
	// create threads and insert, search and delete from list
	for(i = 0; i < NUM_THREADS; i++){
		
		pthread_create(&threads[i], NULL, threadFcn, (void *) myListPtr);
	
	}
	
	pthread_exit(NULL);

	return 0;
	
} // end main
