# Bounded Buffer and Shared Memory Synchronization
Assignment 3 question 2 (A3Q2.c) involves reworking the print server (ie bounded buffer) from A2Q2 using a Sys V shared memory segement instead of 
phtreads that was used in assignment 2. It accepts 2 input arguments from the command line, indicating the number of print servers and
number of print clients. This implementation only works for 1 client and 1 server (as per the instruction). The server and client
communicate using the bounded buffer residing in the shared memory segement, as well as 3 binary semaphores. One is to provide mutual exclusion (semMutex) 
and the other two are used for signalling when the list is NOT full or NOT empty (instead of counting semaphores - similar to A2 implementation).