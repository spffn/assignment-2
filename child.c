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

#define SHMSZ     27
int writeLimit = 0;			// for limiting writes to the files
time_t timeNote;

int main(int argc, char *argv[]){
	
	// process id
	pid_t pid = (long)getpid();
	
	// for timestamps
	timeNote = time(NULL);
	
	// shared memory
	int shmid;
	key_t key = 1001;
	
	// locate the segment
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
	
	char (*mylist)[][200];
	// attach to our data space
	if ((mylist = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
	
	
	int pNum = atoi(argv[1]);		// telling the process which # child it is
	int cLine = 0;					// for tracking lines in file
	int lineToTest = atoi(argv[2]);	// which line to test first
	
	printf("I'm child process #%i!\n", pNum+1);
	fprintf(stderr, "Process ID: %ld\n", pid);
	fprintf(stderr, "Parent ID: %ld\n\n", (long)getppid());
	
	printf("%ld reading line %i from shared memory: \n", pid, lineToTest);
	
	int w = 0, g = 0, v = 0;
	char ch;
	char phrase[200];
	while((*mylist)[w][0] != NULL){
		ch = (*mylist)[w][g];
		
		if(ch == '\0')
		{
			phrase[v] = ch;
			//printf("%s\n", (*mylist)[w]);
			w++;
			g = 0;
			v = 0;
		}
		else {
			phrase[v] = ch;
			g++;
			v++;
		}
	}
	
	printf("%s\n", phrase);
	
	
	/*
	
	int ch = 0;
	if(f != NULL){
		char testLine[150];
		while (fgets(testLine, sizeof testLine, f) != NULL){
			if (cLine == lineToTest){
				if(is_palindrome(testLine, sizeof testLine) != 0){
					
					// YES it was a palindrome
					// check has the write limit been met yet? if no, then go to crit sec
					printf("%ld: Attempting to enter critical section at %s", pid, ctime(&timeNote));
					if(writeLimit > 5){
						printf("%ld: Entering critical section at %s", pid, ctime(&timeNote));
						critical_section(0);
						printf("%ld: Exitting critical section at %s", pid, ctime(&timeNote));
					}
					
					// else end the process
					else { return 0; }
					
					//printf("%ld, #%i || YES Palindrome!: %s \n", pid, cLine, testLine);
					lineToTest = lineToTest + atoi(argv[3]);
				}
				
				else { 
				
					// NOT A PALINDROME
					// check has the write limit been met yet? if no, then go to crit sec
					printf("%ld: Attempting to enter critical section at %s", pid, ctime(&timeNote));
					if(writeLimit > 5){
						printf("%ld: Entering critical section at %s", pid, ctime(&timeNote));
						critical_section(1);
						printf("%ld: Exitting critical section at %s", pid, ctime(&timeNote));
					}
					else { return 0; }
					
					//printf("%ld, #%i  || NO Palindrome!: %s \n", pid, cLine, testLine); 
				}
			}
			else { cLine++; }
		}
	}
	
	fclose(f);*/
	return 0;
}

// check if the passed phrase is a palindrome
int is_palindrome(const char *phrase, unsigned length) {
	
	int i; 
	int c = 0;
	char newStr[50];
	
	for(i = 0; i < length; i++){
		char x = phrase[i];
		if(x == '\0'){ 
			newStr[c] = x; 
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
		writeLimit++;
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
		writeLimit++;
	}
}