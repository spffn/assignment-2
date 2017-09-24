/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIC Processes and Shared Memory */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	
	pid_t ps[5];
	int pCount = 5;
	
    time_t endwait;
    time_t start = time(NULL);
    int timeToWait = 5; // end loop after this time has elapsed
	
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
	printf("LINES: %d\n", countlines("palindromes.txt"));
	
	printf("Trying to make kids:\n");
	for(i = 0; i < 5; i++){
		if ((ps[i] = fork()) < 0) {
			perror(errstr); 
			printf("Fork failed!\n");
			exit(1);
		}
		else if (ps[i] == 0){
			printf("Trying to exec!\n");
			execlp("child", "child", NULL);
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
        printf("loop time is : %s", ctime(&start));
		pid = wait(&status);
		printf("Child #%ld exited with status %i\n", (long)pid, status);
		--pCount;
    }
	
    printf("end time is %s", ctime(&endwait));

    return 0;
}

int countlines(char *filename)
{
	// count the number of lines in the file called filename                                    
	FILE *fp = fopen(filename,"r");
	int ch = 0;
	int lines = 0;

	if (fp == NULL){
		return 0;
	}

	lines++;
	while ((ch = fgetc(fp)) != EOF){
		if (ch == '\n'){
			lines++;
		}
	}
	
	fclose(fp);
	return lines;
}