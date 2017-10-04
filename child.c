/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIX Processes and Shared Memory */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int writeLimit = 0;			// for limiting writes to the files
time_t timeNote;

int main(int argc, char *argv[]){
	
	pid_t pid = (long)getpid();		// process id
	int pNum = atoi(argv[1]);		// telling the process which # child it is
	int lineToTest = atoi(argv[2]);	// which line to test first
	int ln = atoi(argv[3]);			// total lines in file
	
	// shared memory
	int shmid;
	key_t key = 1001;
	
	// locate the segment
	if ((shmid = shmget(key, 10000, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
	
	char (*mylist)[][200];
	// attach to our data space
	if ((mylist = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
	
	
	//fprintf(stderr, "Child #%i || Process ID: %ld || Parent ID: %ld\n", pNum + 1 , pid, (long)getppid());
	
	int i;
	for(i = 0; i < 5; i++){
		
		if(lineToTest > ln){
			printf("Trying to test line higher than exists!\n");
			return 0;
		}
		
		int j;
		char ch;
		char phrase[100];
		for(j = 0; j < 100; j++){
			ch = (*mylist)[lineToTest][j];
			if(ch == '\0') { 
				phrase[j] = ch;
				break;
			}
			else { phrase[j] = ch; }
		}
		
		if(is_palindrome(phrase, strlen(phrase)) != 0){		
			// YES it was a palindrome
			// check has the write limit been met yet? if no, then go to crit sec
			timeNote = time(NULL);
			printf("%ld: Attempting to enter critical section at %s", pid, ctime(&timeNote));
			
			/*
			printf("%ld: Entering critical section at %s", pid, ctime(&timeNote));
			critical_section(0);
			printf("%ld: Exitting critical section at %s", pid, ctime(&timeNote));
			*/
			
			printf("Process %ld, #%i || YES!: %s \n", pid, lineToTest, phrase);
		}			
		else { 	
			// NOT A PALINDROME
			// check has the write limit been met yet? if no, then go to crit sec
			timeNote = time(NULL);
			printf("%ld: Attempting to enter critical section at %s", pid, ctime(&timeNote));

			/*
			printf("%ld: Entering critical section at %s", pid, ctime(&timeNote));
			critical_section(1);
			printf("%ld: Exitting critical section at %s", pid, ctime(&timeNote));
			*/
						
			printf("Process %ld, #%i || NO!: %s \n", pid, lineToTest, phrase);
		}
		
		lineToTest = lineToTest + 20;
	}
	
	return 0;
}

// check if the passed phrase is a palindrome
// start by cleaning the string up and then check it
int is_palindrome(char phrase[], unsigned length) {
	
	int i, c = 0;
	char newStr[50];
	
	for(i = 0; i < length; i++){
		char x = phrase[i];
		if(x == '\0'){ 
			break;
		}
		else if(isspace(x)){ continue; }
		else if(ispunct(x)){ continue; }
		else if (isalpha(x)){
			newStr[c] = tolower(x);
			c++;
		}
		else { newStr[c] = x; c++; }
	}
	
	length = strlen(newStr);
	
    for(i = 0; i < length; i++){
        if(newStr[i] != newStr[length - i - 1]){
            return 0;
	   }
	}
	
    return 1;
}

void critical_section(int yes){
	// for sleeps
	int r;
	srand(time(NULL));
	
	if(yes == 0){
		// the line was a palindrome
		// generate val between 0 and 2 to sleep before write
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
		
		// write to palin.out
		
		// generate val between 0 and 2 to sleep before exit
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
	}
	else {
		// the line was NOT a palindrome
		// generate val between 0 and 2 to sleep before write
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
		
		// write to nopalin.out
		
		// generate val between 0 and 2 to sleep before exit
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
	}
}