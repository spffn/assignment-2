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
pid_t pid;

int main(int argc, char *argv[]){
	
	pid = (long)getpid();			// process id
	int pNum = atoi(argv[1]);		// telling the process which # child it is
	int lineToTest = atoi(argv[2]);	// which line to test first
	int ln = atoi(argv[3]);			// total lines in file
	
	// for concurrency
	enum state{
		idle, want_in, in_cs
	};
	int n = 5;
	extern int turn;
	extern state flag[n];
	int local, result;
	
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
		
		result = is_palindrome(phrase, strlen(phrase));		// 1 = yes
			
		timeNote = time(NULL);
		fprintf(stderr, "%ld: Attempting to enter critical section at %s", pid, ctime(&timeNote));
			
		do {
			flag[pNum] = want_in;	// raise flag
			local = turn;			// set local
			
			// wait till turn
			while(j != pNum){
					j = (flag[j] != idle) ? turn : (j + 1) % n;
			}
			
			// delcare intent
			flag[pNum] = in_cs;
			
			// check no one else is there
			for (j = 0; j < n; j++){
				if((j != i) && (flag[j] == in_cs)){
					break;
				}
			}
			
		} while ((j < n) || (turn != i && flag[turn] != idle));
		
		// assign self the turn and go into crit section, printing to stderr
		// when they enter and exit
		turn = pNum;
		fprintf(stderr, "%ld: Entering critical section at %s", pid, ctime(&timeNote));
		critical_section(0, lineToTest, phrase);
		fprintf(stderr, "%ld: Exitting critical section at %s", pid, ctime(&timeNote));
		
		// exit
		j = (turn + 1) % n;
		while(flag[j] == idle){
			j = (j + 1) %n;
		}
		
		// give turn to next waiting process, change own flag to idle
		turn = j;
		flag[pNum] = idle;
		
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

void critical_section(int yes, int index, char phrase[]){
	// for sleeps
	int r;
	srand(time(NULL));
	
	if(yes == 0){
		// the line was a palindrome
		// generate val between 0 and 2 to sleep before write
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
		
		FILE * fil;
		fil = fopen("palin.out", "a");
		fprintf(fil,"%ld \t %i \t %s", pid, index, phrase);
		fclose(fil);
		
		// generate val between 0 and 2 to sleep before exit
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
	}
	else {
		// the line was NOT a palindrome
		// generate val between 0 and 2 to sleep before write
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
		
		FILE * fil;
		fil = fopen("nopalin.out", "a");
		fprintf(fil,"%ld \t %i \t %s", pid, index, phrase);
		fclose(fil);
		
		// generate val between 0 and 2 to sleep before exit
		r = rand() % (2 - 0 + 1) + 0;
		sleep(r);
	}
}