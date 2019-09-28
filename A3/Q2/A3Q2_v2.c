#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 4
#define MAX_FILENAME_SIZE 64
#define FILE_SIZE_MAX 40000
#define FILE_SIZE_MIN 500

typedef struct PR {
	long clientID; // client id 
	char fileName[MAX_FILENAME_SIZE]; // file name (ptr unusable in shared memory)
	int fileSize; // file size
} PrintRequest;

typedef struct BB {
	PrintRequest BoundedBuffer[MAX_BUFFER_SIZE]; // bounded buffer
	int numElts; // number of elements
	int start; // track start of list
	int end;// track end of list
} BoundedBuffer;

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

void shiftEltsFwd(BoundedBuffer *sharedBBuff){
	
	int i; // FOR loop
	
	for(i = sharedBBuff->numElts-1; i >= 0; i--){
		
		sharedBBuff->BoundedBuffer[i+1] = sharedBBuff->BoundedBuffer[i];
		
	} /* End of For */

} /* End of ShiftEltsFwd*/

void enQ(PrintRequest printReq, BoundedBuffer *sharedBBuff){
	
	// Insert item into start of queue
	shiftEltsFwd(sharedBBuff);
	sharedBBuff->BoundedBuffer[sharedBBuff->start] = printReq;
	sharedBBuff->end++;
	sharedBBuff->numElts++;
	
	
	// Enqueued print request info
	printf("***Inserted to Queue***\n");
	printf("Client ID: %ld\n", sharedBBuff->BoundedBuffer[sharedBBuff->start].clientID);
	printf("Inserting: %s\n\n", sharedBBuff->BoundedBuffer[sharedBBuff->start].fileName);
	
		
} /* End of enQ */

PrintRequest deQ(BoundedBuffer *sharedBBuff){

	PrintRequest deQPrintReq;

	//Get Time
	time_t currTime; // current time
	time(&currTime);

	// Construct and store print request for printing
	deQPrintReq.clientID = sharedBBuff->BoundedBuffer[sharedBBuff->end].clientID;
	strcpy(deQPrintReq.fileName, sharedBBuff->BoundedBuffer[sharedBBuff->end].fileName);
	deQPrintReq.fileSize = sharedBBuff->BoundedBuffer[sharedBBuff->end].fileSize;
	
	// Dequeued print request info
	printf("***Removing from Queue***\n");
	printf("Printer ID: %ld\n", getpid());
	printf("Print Request Info\n");
	printf("Client ID: %ld\n", deQPrintReq.clientID);
	printf("File Name: %s\n", deQPrintReq.fileName);
	printf("File Size: %d\n", deQPrintReq.fileSize);
	printf("Current Time: %s\n", ctime(&currTime));
	
	// Dequeue item by adjusting end ptr
	sharedBBuff->end--;
	sharedBBuff->numElts--;
	
	return deQPrintReq;

} /* End of deQ */

void printServer(){
	
	// Shared Data Segment
	key_t key;
	int sharedSegID;
	BoundedBuffer *sharedBBuff;
	size_t sharedSegSize;
	
	// Semaphores
	sem_t *semMutex;
	sem_t *semNotFull;
	sem_t *semNotEmpty;
    
	// Create semaphores
	semMutex = sem_open("mutex", O_CREAT, 0644, 1);
	semNotFull = sem_open("notFull", O_CREAT, 0644, 0);
	semNotEmpty = sem_open("notEmpty", O_CREAT, 0644, 0);
	
	// Create key for shared memory segment
	key = 7686561;

	// Allocate size of shared segment
	sharedSegSize = sizeof(sharedBBuff);
	
	 // Create the segment (IPC_CREAT)
	sharedSegID = shmget(key, sharedSegSize, IPC_CREAT | 0666);	
	
	// Attach the segment
	sharedBBuff = shmat(sharedSegID, NULL, 0);
	
	// Initialize Bounded Buffer
	sharedBBuff->numElts = 0;
	sharedBBuff->start = 0;
	sharedBBuff->end = -1;
	
	while(1){ // infinite loop
		
		// Wait until not empty in order to remove element
		while(sharedBBuff->numElts == 0){
			
			sem_wait(semNotEmpty);

		} /* End of While */
		
		sem_wait(semMutex); // lock
		
		// perform dequeue
		PrintRequest deQPrintReq = deQ(sharedBBuff); 
				
		sem_post(semMutex); // unlock
		
		sem_post(semNotFull); // signal not full
		
		// sleep for time specified by file size/4000
		int sleepTime = deQPrintReq.fileSize/4000;
		sleep(sleepTime);
		
		//Get Time
		time_t currTime; // current time
		time(&currTime);
		
		// Finished print request info
		printf("***Finished Printing***\n");
		printf("Printer ID: %ld\n", getpid());
		printf("Print Request Info\n");
		printf("Client ID: %ld\n", deQPrintReq.clientID);
		printf("File Name: %s\n", deQPrintReq.fileName);
		printf("File Size: %d\n", deQPrintReq.fileSize);
		printf("Current Time: %s\n", ctime(&currTime));
			
	} /* End of While */	
	
	
} /* End of printServer */

void printClient(){
	
	int i; // FOR loop
	char temp[MAX_FILENAME_SIZE]; // temp char array to construct file name
	PrintRequest printReq; // print request
	
	// Shared Data Segment
	key_t key;
	int sharedSegID;
    BoundedBuffer *sharedBBuff;
	size_t sharedSegSize;
	
	// Semaphores
	sem_t *semMutex;
	sem_t *semNotFull;
	sem_t *semNotEmpty;
    
	// Create semaphores
	semMutex = sem_open("mutex", O_CREAT, 0644, 1);
	semNotFull = sem_open("notFull", O_CREAT, 0644, 0);
	semNotEmpty = sem_open("notEmpty", O_CREAT, 0644, 0);

	// Create key for shared memory segment
	key = 7686561;

	// Allocate size of shared segment
	sharedSegSize = sizeof(sharedBBuff);

	// Allow for server to create shared segment
	sleep(1); 

	// Locate the segment, already created by the "server" process
	sharedSegID = shmget(key, sharedSegSize, 0666);

	// Attach the segment to our address space
	sharedBBuff = shmat(sharedSegID, NULL, 0);
	
	// Construct beginning of file name
	sprintf(temp, "%s%s", "File", "-");
	sprintf(temp, "%s%ld", temp, getpid());
	sprintf(temp, "%s%s", temp, "-");
	
	// Insert print request into BB
	for(i = 0; i < 6; i++){ // loop 6 times
	
		// Finish file name construction
		char fName[MAX_FILENAME_SIZE]; 
		sprintf(fName, "%s%d", temp, i+1);
		
		// Construct printRequest
		printReq.clientID = getpid();
		strcpy(printReq.fileName, fName);
		printReq.fileSize = randNum(FILE_SIZE_MAX, FILE_SIZE_MIN);
		
		// Wait until buffer is not not full
		while(sharedBBuff->numElts == MAX_BUFFER_SIZE) {
			
			sem_wait(semNotFull);
		
		} /* End of While*/
		
		sem_wait(semMutex); // lock
		
		// Enqueue in buffer
		enQ(printReq, sharedBBuff);
		
		sem_post(semMutex); // unlock
		
		sem_post(semNotEmpty); // signal not empty
		
		// sleep for 0, 1, 2, or 3 seconds
		int randSleep = randNum(3,0);
		sleep(randSleep);
	
	} /* End of For */
	
	// Detach shared memory segment and close semaphores
	shmdt(sharedBBuff);
	sem_close(semMutex);
	sem_close(semNotFull);
	sem_close(semNotEmpty);	
	
} /* End of printClient */

int main (int argc, char *argv[]) {

	// DECLARATIONS
	int numPrintClients;
	int numPrinters;
	long pid;
	char correctInput;
	int i;
	
	// Initialize
	correctInput = 0;
	
	// check for correct args
	if (argv[1] != NULL && argv[2] != NULL){
		
		// retrieve arguments
		numPrintClients = atoi(argv[1]);
		numPrinters = atoi(argv[2]);
		
		if(numPrintClients == 1 && numPrinters == 1){
		
			// establish correct input
			correctInput = 1;
		
		}else{
			
			printf("Only supports 1 printClient and 1 printServer.\n");
			
		}/* End of If-Else */
	}else{
		
		printf("Incorrect amount of arguments arguments.\n");
	
	} /* End of If-Else */

	// check for correct input
	if(correctInput){
	
		// Create printer process & print client process
		pid = fork();
		
		// Send processes to appropriate functions
		if(pid == 0){
			
			printClient(); // child process is client
			
		}else{
			
			printServer(); // parent process is server
			
		} /* End of If-Else */
	
	} /* End of If */
	
	// wait for child
	wait(NULL);
		
} /* End of Main */