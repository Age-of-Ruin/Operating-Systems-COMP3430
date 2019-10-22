# Kill Signal
Assignment 3 question 3 involves adding a singal handler to the print server (created in question 2) in order to catch the
USRSIG1 signal sent from the shutdown.c file. A3Q3.c behaves exactly the same as A3Q2.c, accepting two arguments determining the number of 
print clients and print servers (again, this implementation only supports one of each). The shutdown.c file takes 1 argument which indicates
the process that the USRSIG1 signal will be sent to (using the kill() system call).