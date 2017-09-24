/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIC Processes and Shared Memory */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    time_t endwait;
    time_t start = time(NULL);
    int timeToWait = 60; // end loop after this time has elapsed
	
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
				fprintf(stderr, "\tDefault is 10 seconds. Must be a number. \n");
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

    while (start < endwait)
    {
        /* Do stuff while waiting */
        sleep(1);   // sleep 1s.
        start = time(NULL);
        printf("loop time is : %s", ctime(&start));
    }

    printf("end time is %s", ctime(&endwait));

    return 0;
}