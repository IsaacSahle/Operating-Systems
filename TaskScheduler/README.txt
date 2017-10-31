Author: Isaac Sahle (V00816592)

My implementation of the airline check-in system and sample test file "customers.txt" is provided. My program slightly differs from
my design documentation in the following ways ...

- Instead of having four queue locks, I now have a single lock for all four queues
- I added a structure for the clerks to record the customer id that they are serving, and a customer
  is then able to identify which clerk is setving them. This added an extra mutex lock.

The reasons for the removal of 4 queue locks and replacing it with one was due to the fact that there were deadlock issues
when a customer would try to obtain a queue lock and the lock for the structure containing the queue lengths. Using one queue lock
removed the possibility for this scenario to occur.

To compile: run the "make" command
To execute: run the command ./ACS customers.txt


(BONUS:) If you wanted to run some trials for a different number of queues and/or clerks toggle the values
 for NCLERK and NQUEUE in airline.h :)
