// include stdio.h to get a definition of NULL
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// include header file for linked list data structure definitions
# include "list.h"

pthread_mutex_t mutex;

// Note that in the following routines I have name variables to that
// the use of pointers is explicit. (e.g. 'listPtr' instead of 'list',
// 'newNodePtr' instead of 'newNode', 'currPtr' instead of 'curr', etc.
// If you are prone to errors with C pointers, doing this in your own
// code may be helpful.


// routine to insert a new <key,value> pair into the list.
void listInsert(listNodePtr *listPtr, long key, long value) {
        
	pthread_mutex_lock(&mutex); // MUTEX LOCK
	
	listNodePtr	newNodePtr;

	newNodePtr=(listNodePtr) malloc(sizeof(listNode));
	newNodePtr->key=key;
	newNodePtr->data=value;
	if (*listPtr==NULL) {
		// first insertion into empty list
		newNodePtr->next=NULL;
		*listPtr=newNodePtr;
	} else {
		// insert new node at head of list - easy
		newNodePtr->next=*listPtr;
		*listPtr=newNodePtr;
	}

	pthread_mutex_unlock(&mutex); // MUTEX UNLOCK
	
} // end listInsert



// routine to delete a node with the given key from the list.
void listDelete(listNodePtr *listPtr, long key) {

	pthread_mutex_lock(&mutex); // MUTEX LOCK

	listNodePtr	currPtr,prevPtr;

	prevPtr=NULL;
	currPtr=*listPtr;
	while (currPtr!=NULL) {
		if (currPtr->key==key) {
			// found node to delete
			if (prevPtr==NULL) {
				// deleting node at head of list
				*listPtr=currPtr->next;
				free(currPtr);
			} else {
				// deleting node past head of list
				prevPtr->next=currPtr->next;
				free(currPtr);
			}

			pthread_mutex_unlock(&mutex); // MUTEX UNLOCK

			return;
		} else {
			prevPtr=currPtr;
			currPtr=currPtr->next;
		}
	}

    pthread_mutex_unlock(&mutex); // MUTEX UNLOCK

} // end listDelete



// routine to return the data from the node with the given key.
long listSearch(listNodePtr listPtr, long key) {

	pthread_mutex_lock(&mutex); // MUTEX LOCK

	listNodePtr	currPtr;
	long		result;
	
	currPtr=listPtr;
	while (currPtr!=NULL) {
		if (currPtr->key==key) {       	// As you convert this code for threads,
			result=currPtr->data;	// think about why I didn't just code
			
			pthread_mutex_unlock(&mutex); // MUTEX UNLOCK
			
			return(result);		// 'return(curr->data)' in this if statement.
		} else {
			currPtr=currPtr->next;
		}
	}

	pthread_mutex_unlock(&mutex); // MUTEX UNLOCK

	return (-1); 	// indicates "not found"
} // end listSearch
