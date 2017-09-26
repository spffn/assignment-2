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

#define SHMSZ     27

int main(int argc, char *argv[]){
	
	// the processes to create
	pid_t ps[3];
	int pCount = 3;
	
	// shared memory
	int shmid;
	key_t key;
	char *shm, *s;
	
	// the timer information
    time_t endwait;
    time_t start = time(NULL);
    int timeToWait = 10; 		// end loop after this time has elapsed
	
	// for printing errors
	char errstr[50];
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
	
	// shared mem is called '1001' which is a palindrome
	key = 1001;
	
	// create segment
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
	
	// attach segment to data space
	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
	
	// put crap in it
	s = shm;
	for (c = 'a'; c <= 'z'; c++){
        *s++ = c;
	}
    *s = NULL;
	
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
	
    while (start < endwait || pCount > 0)
    {  
        /* Do stuff while waiting */
        start = time(NULL);
		sleep(1);
		pid = wait(&status);
		--pCount;
    }
	
	if(start < endwait && pCount > 0){
		printf("Out of time! Terminating %i children due to constraints.", pCount);
	}
	
    printf("end time is %s", ctime(&endwait));
	
	printf("Cleaning up shared memory.\n");
	shmctl(shmid, IPC_RMID, NULL);

    return 0;
}