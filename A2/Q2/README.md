# Kernal vs User Thread Visualization
Assignment 2 Question 2 attempts to visualize whether pthreads are visible to the kernel. This is accomplished using two threads: 

- Thread 1 will read input from stdin and exit upon receiving input.
- Thread 2 will sleep for 5 seconds then print 10 statements and exit.

Examining the behavior of the program, it is evident that each thread operates independently and are not blocked by one another.
This implies that pthreads ARE VISIBLE to the kernel, behaving similar to kernel level threads.