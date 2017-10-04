--- README ---

Taylor Clark
CS 4760
Assignment #2
Concurrent UNIX Processes and Shared Memory

---

THIS PROJECT DOES NOT DO PROPER CONCURRENCY OR SIGNAL HANDLING.
All the rest of the requirements are there, and the skeleton for code relating to
concurrency is in place. However, it is not properly implemented. I understand that
I am turning in this project with this missing.

As a general description, this project checks for palindromes from a file. The information in this file is read into shared memory, then lines are divided up and passed to the 20 child processes to check for palindromes. Regardless of whether a palindrome is found, the process will write to one of two files, after checking that no other process is already doing so. If a timelimit is hit (edittable by the user), then all processes will be killed and shared memory cleaned up.


--- TO COMPILE ---
Run the makefile to compile both master and palin.

--- TO RUN ---
./(program name) -(any required command line arguments) (their values, if needed)
EX: ./master -t 20

--- GITHUB ---
I am still not the most proficient at GitHub but I am learning.

	https://github.com/spffn/assignment-2


--- DESCRIPTIONS ---

-- master
	This is the main process of the program. It creates, attaches and write to shared memory, all the lines from the file of palindromes. It then forks off 20 and gives each process a specific line.
	
	Master will run for a designated amount of time, which is by default 60 seconds. If the limit for this time is reached, all remaining children will be killed.
	
	This process has three accepted command line arguments:
	
		-t: Will set the time in seconds to allow the program to run.
			It requires an argument. It is required for the program to run.
			Default is 60 seconds.
			Ex. -t 40
		-l: Will allow you to specify the filename to pull palindromes from.
			By default, it is "palindromes.txt".
			Please include the extension.
			Ex. -l palins.txt
		-h: Show help. This shows all the accepted command lines arguments for this
			version of the program.
			Not required for program to run.

-- palin
	This is the code which is exec'd by master:
	
		palin 	id 	xx
	
	Where id is the sequential identification number for the child and xx is the index number of the string to be tested in shared memory.
	
	It checks this given string from shared memory for a palindrome, and regardless of whether it finds one or not, it writes to the appropriate file, palin.out or nopalin.out. The process may only write a max of 5 time to any file, before it exits.
	
	The format for each file write is:
	
		PID 	Index 	String
		
	Each time the process attempts to enter its critical section, a note is written out with the time. If the process actually enters its critical section, another note is written, same when it leaves. In the critical section, the process waits for a random amount of time between 0 and 2 seconds before writing, then waiting again before leaving.