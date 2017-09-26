/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIC Processes and Shared Memory */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ     27

int main(int argc, char *argv[]){
	
	pid_t pid = (long)getpid();
	
	// shared memory
	int shmid;
	key_t key;
	char *shm, *s;
	key = 1001;
	
	// locate the segment
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
	
	// attach to our data space
	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
	
	
	int pNum = atoi(argv[1]);	// telling the process which # child it is
	int cLine = 0;				// for tracking lines in file
	int lineToTest = pNum;		
	int writeLimit = 0;			// for limiting writes to the files
	
	printf("I'm child process #%i!\n", pNum+1);
	fprintf(stderr, "Process ID: %ld\n", pid);
	fprintf(stderr, "Parent ID: %ld\n\n", (long)getppid());
	printf("--------------\n");
	
	
	printf("%ld reading from shared memory: ", pid);
	for (s = shm; *s != NULL; s++){
        putchar(*s);
	}
    putchar('\n');
	
	
	FILE * f = fopen("palindromes.txt", "r");
	
	int ch = 0;
	if(f != NULL){
		char testLine[150];
		while (fgets(testLine, sizeof testLine, f) != NULL){
			if (cLine == lineToTest){
				if(is_palindrome(testLine, sizeof testLine) != 0){
					//printf("%ld, #%i || YES Palindrome!: %s \n", pid, cLine, testLine);
					lineToTest = lineToTest + atoi(argv[3]);
				}
				else { //printf("%ld, #%i  || NO Palindrome!: %s \n", pid, cLine, testLine); 
				}
			}
			else { cLine++; }
		}
	}
	
	fclose(f);
	return 0;
}

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