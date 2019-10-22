# First Fit and Free Space Amalgamation
A4Q1 involved to extending the skelton code (mymem.c) to facilitate chained free space amalgamation. This involves managing the linked list pointing to free spaces
when a myfree() call is made. By searching the free list and utilizing prevNode and nextNode (ie nodes on the free list adjacent to the memory locations 
to be freed), we can identify if any free spaces that are adjacent and combine them with the desired freed space, as well as resolve the location the linked list.
We then manage pointers to these free locations and combine any adjacent free memory locations by manipulating these list pointers and sizes of those free spaces.