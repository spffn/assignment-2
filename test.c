/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIC Processes and Shared Memory */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	
	pid_t ps[1];
	int pCount = 1;
	
    time_t endwait;
    time_t start = time(NULL);
    int timeToWait = 10; // end loop after this time has elapsed
	
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
			// if no argument is given for n, print an error and end.
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

    endwait = start + timeToWait;

    printf("start time is : %s", ctime(&start));
	
	printf("Trying to make kids:\n");
	for(i = 0; i < pCount; i++){
		if ((ps[i] = fork()) < 0) {
			perror(errstr); 
			printf("Fork failed!\n");
			exit(1);
		}
		else if (ps[i] == 0){
			// pass to the execlp, the name of the code to exec
			// the # child it is
			char cProNum[5];
			sprintf(cProNum, "%i", i);
			char totalProNum[5];
			sprintf(totalProNum, "%i", pCount);
			execlp("child", "child", cProNum, totalProNum, NULL);
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
		sleep(5);
		pid = wait(&status);
		printf("Child #%ld exited with status %i\n", (long)pid, status);
		--pCount;
    }
	
    printf("end time is %s", ctime(&endwait));

    return 0;
}