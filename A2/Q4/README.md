# Bounded Buffer and Multithreaded Synchronization
Assignment 2 Question 4 simulates a multithreaded client-server architecture that resembles clients sending print requests to multiple printers.
This program attempts to accomplish this by implementing a solution to the bounded buffer problem using a single pthread mutex, two condition variables
and a queue (ie bounded buffer). The mutex ensures that only a single thread is operating on the list at one time, and the condtion variables
help to control the flow of the code by ensuring that elements are only inserted or removed when list can accomodate it (ie insert when the list is not full
and remove when the list is not empty).