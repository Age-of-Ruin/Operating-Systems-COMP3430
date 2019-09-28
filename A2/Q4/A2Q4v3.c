#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_BUFFER_SIZE 4
#define MAX_FILENAME_SIZE 100
#define FILE_SIZE_MAX 40000
#define FILE_SIZE_MIN 500

typedef struct PR {
	long clientID; // client id (ie pthread_self)
	char *fileName; // file name
	int fileSize; // file size
} PrintRequest,*PrintRequestPTR;

typedef struct BB {
	PrintRequest BoundedBuffer[MAX_BUFFER_SIZE]; // bounded buffer
	int numElts; // number of elements
	int start; // track start of list
	int end;// track end of list
} BoundedBuffer,*BoundedBufferPTR;

typedef struct monitor{
	pthread_mutex_t mutex; // mutex to provide mutual exclusion
	pthread_cond_t notFull; // condition variable to check if queue is notFull
	pthread_cond_t notEmpty; // condition variable to check if queue is not Empty
} Monitor, *MonitorPTR;


//GLOBAL DATA STRUCTURES

// Declare and Initialize Bounded buffer
BoundedBuffer bBuff;

// Declare monitor
Monitor myMonitor;


unsigned int randNum(int max, int min){
	
    unsigned int randNo;
    unsigned int range = max - min + 1;
    unsigned int buckets = RAND_MAX / range;

	//Generate random number using time as seed
	srand (time(NULL));
	randNo = rand();
	
	// Adjust for extreme case (randNo == RAND_MAX would return 1 over max value)
	// and RAND_MAX + 1 causes integer overflow
	if(randNo == RAND_MAX)
		randNo--;

    return min + (randNo / buckets);
	
} /* End of randNum */

void shiftEltsFwd(){
	
	int i; // FOR loop
	
	for(i = bBuff.numElts-1; i >= 0 && i < MAX_BUFFER_SIZE ; i--){
		
		bBuff.BoundedBuffer[i+1] = bBuff.BoundedBuffer[i];
		
	} /* End of For */

} /* End of ShiftEltsFwd*/

void enQ(PrintRequest printReq){
	
	pthread_mutex_lock(&myMonitor.mutex); //LOCK MUTEX
	
	// Wait until buffer is not not full
	while(bBuff.end == MAX_BUFFER_SIZE-1) {
			
		pthread_cond_wait(&myMonitor.notFull, &myMonitor.mutex);
		
	} /* End of While*/
		
	// Insert item into start of queue
	shiftEltsFwd();
	bBuff.BoundedBuffer[bBuff.start] = printReq;
	bBuff.end++;
	bBuff.numElts++;
	
	// Signal that queue is no longer empty (item can be removed)
	pthread_cond_signal(&myMonitor.notEmpty);
	
	// Enqueued print request info
	printf("***Inserted to Queue***\n");
	printf("Client ID: %ld\n", bBuff.BoundedBuffer[bBuff.start].clientID);
	printf("Inserting: %s\n\n", bBuff.BoundedBuffer[bBuff.start].fileName);
	
	pthread_mutex_unlock(&myMonitor.mutex); //UNLOCK MUTEX	
		
} /* End of enQ */

PrintRequestPTR deQ(){
	
	pthread_mutex_lock(&myMonitor.mutex); //LOCK MUTEX
	
	PrintRequestPTR deQPrintReqPtr = NULL;
	
	// Wait until not empty in order to remove element
	while(bBuff.end == -1){
		
		pthread_cond_wait(&myMonitor.notEmpty, &myMonitor.mutex);
	
	} /*End of If-Else */
		
	//Get Time
	time_t currTime; // current time
	time(&currTime);
	
	// Construct and store print request for printing
	deQPrintReqPtr = malloc(sizeof(PrintRequest));
	deQPrintReqPtr->clientID = bBuff.BoundedBuffer[bBuff.end].clientID;
	deQPrintReqPtr->fileName = bBuff.BoundedBuffer[bBuff.end].fileName;
	deQPrintReqPtr->fileSize = bBuff.BoundedBuffer[bBuff.end].fileSize;
	
	// Dequeued print request info
	printf("***Removing from Queue***\n");
	printf("Printer ID: %ld\n", pthread_self());
	printf("Print Request Info\n");
	printf("Client ID: %ld\n", deQPrintReqPtr->clientID);
	printf("File Name: %s\n", deQPrintReqPtr->fileName);
	printf("File Size: %d\n", deQPrintReqPtr->fileSize);
	printf("Current Time: %s\n", ctime(&currTime));
	
	// Dequeue item by adjusting end ptr
	bBuff.end--;
	bBuff.numElts--;
	
	// Signal that queue is no longer full (item can be inserted)
	pthread_cond_signal(&myMonitor.notFull);
	
	pthread_mutex_unlock(&myMonitor.mutex); //UNLOCK MUTEX
	
	return deQPrintReqPtr;
	
} /* End of deQ */

void *printServer(void *args){
	
	while(1){ // infinite loop
	
		PrintRequestPTR deQPrintReqPtr = deQ(); // perform dequeue
		
		if(deQPrintReqPtr != NULL){
			
			// sleep for time specified by file size/40000
			int sleepTime = deQPrintReqPtr->fileSize/4000;
			sleep(sleepTime);
			
			//Get Time
			time_t currTime; // current time
			time(&currTime);
			
			// Finished print request info
			printf("***Finished Printing***\n");
			printf("Printer ID: %ld\n", pthread_self());
			printf("Print Request Info\n");
			printf("Client ID: %ld\n", deQPrintReqPtr->clientID);
			printf("File Name: %s\n", deQPrintReqPtr->fileName);
			printf("File Size: %d\n", deQPrintReqPtr->fileSize);
			printf("Current Time: %s\n", ctime(&currTime));
			
		}

	} /* End of While */
	
	pthread_exit(NULL);
	
} /* End of printServer */

void *printClient(void *args){
	
	int i; // FOR loop
	char temp[MAX_FILENAME_SIZE]; // temp char array to construct file name
	PrintRequest printReq; // print request
	
	// Construct beginning of file name
	sprintf(temp, "%s%s", "File", "-");
	sprintf(temp, "%s%ld", temp, pthread_self());
	sprintf(temp, "%s%s", temp, "-");
	
	// Insert print request into BB
	for(i = 0; i < 6; i++){ // loop 6 times
		
		// Finish file name construction
		char fName[MAX_FILENAME_SIZE]; 
		sprintf(fName, "%s%d", temp, i);
		
		// Construct printRequest
		printReq.clientID = pthread_self();
		printReq.fileName = strdup(fName);
		printReq.fileSize = randNum(FILE_SIZE_MAX, FILE_SIZE_MIN);
		
		// Enqueue into BB
		enQ(printReq);
		
		// sleep for 0, 1, 2, or 3 seconds
		int randSleep = randNum(3,0);
		sleep(randSleep);
	
	} /* End of For */
	
	pthread_exit(NULL);
	
} /* End of printClient */

int main (int argc, char *argv[]) {

	// DECLARATIONS
	int numPrintClients;
	int numPrinters;
	
	char correctInput;
	
	int i;
	
	// Initialize Bounded Buffer
	correctInput = 0;
	bBuff.numElts = 0;
	bBuff.start = 0;
	bBuff.end = -1;
	
	// check for correct args
	if (argv[1] != NULL && argv[2] != NULL){
		
		// retrieve arguments
		numPrintClients = atoi(argv[1]);
		numPrinters = atoi(argv[2]);
		
		// establish correct input
		correctInput = 1;
		
	}else{
		
		printf("Incorrect amount of arguments arguments.\n");
	
	} /*End of If-Else */

	// check for correct input
	if(correctInput){
	
		// Create printer threads for managing printers
		pthread_t printerThreads[numPrinters];
		for(i = 0; i < numPrinters; i++){
			
			pthread_create(&printerThreads[i], NULL, printServer, NULL);		
		
		} /* End of For */
		
		// Create client threads for managing print requests
		pthread_t printClientThreads[numPrintClients];
		for(i = 0; i < numPrintClients; i++){
			
			pthread_create(&printClientThreads[i], NULL, printClient, NULL);		
		
		} /* End of For */
	
	} /* End of If */
	
	//printf("Main Thread Exiting\n");
	
	pthread_exit(NULL);	// Main thread exit
		
} /* End of Main */