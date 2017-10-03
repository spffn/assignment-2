/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIC Processes and Shared Memory */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>

#define SHMSZ     27

char errstr[50];
off_t fsize(void);

int main(int argc, char *argv[]){
	
	// the processes to create
	pid_t ps[3];
	int pCount = 1;
	
	// shared memory
	int shmid;
	key_t key;
	char *shm;
	
	// the timer information
    time_t endwait;
    time_t start = time(NULL);
    int timeToWait = 5; 		// end loop after this time has elapsed
	
	// for printing errors
	snprintf(errstr, sizeof errstr, "%s: Error: ", argv[0]);
	int i, c;
	opterr = 0;	

	// parse the command line arguments
	while ((c = getopt(argc, argv, ":t:h")) != -1) {
		switch (c) {
			// sets the amount of time to let a program run until termination
			case 't':
				timeToWait = atoi(optarg);
				break;
			// show help
			case 'h':
				fprintf(stderr, "\n----------\n");
				fprintf(stderr, "HELP LIST: \n");
				fprintf(stderr, "-t: \n");
				fprintf(stderr, "\tSets the amount of seconds to wait before terminating program. \n");
				fprintf(stderr, "\tDefault is 60 seconds. Must be a number. \n");
				fprintf(stderr, "\tex: -t 60 \n");
				fprintf(stderr, "-h: \n");
				fprintf(stderr, "\tShow help, valid options and required arguments. \n");
				fprintf(stderr, "----------\n\n");
				break;
			// if no argument is given, print an error and end.
			case ':':
				perror(errstr);
				fprintf(stderr, "-%s requires an argument. \n", optopt);
				return 1;
			// if an invalid option is caught, print that it is invalid and end
			case '?':
				perror(errstr);
				fprintf(stderr, "Invalid option(s): %s \n", optopt);
				return 1;
		}
	}

	// compute time to end program
    endwait = start + timeToWait;

    printf("Start time is : %s", ctime(&start));
	
	
	// SHARED MEMORY STUFF
	// shared mem is called '1001' which is a palindrome
	key = 1001;
	int lc = count();			// line count of file
	
	// create segment of appropriate size to hold all the info from file
	if ((shmid = shmget(key, (fsize() + lc + (lc + 1) * sizeof(char *)), IPC_CREAT | 0666)) < 0) {
        perror("shmget failed");
        exit(1);
    }
	
	// attach segment to data space
	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }
	
	char **mylist;				// shared memory list
	mylist = (char **)shm;		// the beginning of the shared mem block
	mylist[lc] = NULL;			// when mylist[n] returns NULL its because you
								// have reached its end
								
	char * buf;
	buf = (char *)(&mylist[lc+1]);  // points to the spot in memory block right
									// after where the mylist ends
	
	// open file and put the lines into shared memory
	FILE * f = fopen("palindromes.txt", "r");
	if (f == 0)
    {
		perror(errstr);
        fprintf(stderr, "Failed to open file for reading\n");
        return 1;
    }
	
	int w = 0;
	while(!feof(f)) {
		mylist[w++] = buf;
		fgets(buf, fsize(), f);
		buf += strlen(buf) +1;
	}
	fclose(f);
	
	for(i = 0; i < pCount; i++){
		int lineToTest;			// for tracking lines the process is assigned
		
		if ((ps[i] = fork()) < 0) {
			perror(errstr); 
			printf("Fork failed!\n");
			exit(1);
		}
		else if (ps[i] == 0){
			// pass to the execlp, the name of the code to exec
			// the # child it is
			char id[5];
			sprintf(id, "%i", i);
			char totalProNum[5];
			sprintf(totalProNum, "%i", pCount);
			char xx[5];
			sprintf(xx, "%i", 0);
			execlp("palin", "palin", id, xx, totalProNum, NULL);
			perror(errstr); 
			printf("execl() failed!\n");
			exit(1);
		}
	}
	
	int status;
	pid_t pid;
	int pC, tProc;		// counter for for loop in while
						// total active processes
	tProc = pCount;		// intially set to same # as spawned processes
		
	// master waits until the time runs out or the children all end
    while ((start < endwait) && (tProc > 0))
    {  
		// check time while running
        start = time(NULL);
		sleep(1);
		// keep checking to see if all the children still exist
		for(pC = 0; pC < pCount; pC++){
			// if call succeeds, process with this pid still exists
			if(kill(ps[pC], 0)) { break; }
			// else its already dead, so decrement total process count
			else { tProc--; }
		}
    }
	time_t outOfLoop = time(NULL);
	
	// if the while loop ran out and there were still children
	// then print a notice and kill them
	if(start < endwait && tProc > 0){
		printf("Out of time! Terminating %i children due to constraints.", tProc);
		for(pC = 0; pC < pCount; pC){
			if(kill(ps[pC], 0)) { }
			else { kill(ps[pC], SIGKILL); }
		}
	}
	
    printf("end time is %s", ctime(&endwait));
	
	printf("Cleaning up shared memory.\n");
	shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

// get the size of the file
off_t fsize(){
	struct stat st;

    if (stat("palindromes.txt", &st) == 0){
        return st.st_size;
	}

	perror(errstr); 
    printf("Cannot determine size of palindromes.txt\n");

    return 1;
}

// get the # of lines in the file
int count(){
	FILE *f = fopen("palindromes.txt","r");
	int lines = 0;
	int ch = 0;
	
	if (f == NULL){
		perror(errstr); 
		printf("Cannot open palindromes.txt\n");
		return 1;
	}
	
	while(!feof(f))
	{
		ch = fgetc(f);
		if(ch == '\n')
		{
			lines++;
		}
	}
	
	fclose(f);
	return lines;
}