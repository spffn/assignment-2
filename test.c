/* Taylor Clark
CS 4760
Assignment #2
Concurrent UNIX Processes and Shared Memory */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>

char errstr[50];
off_t fsize(char []);

int main(int argc, char *argv[]){
	
	// the processes to create
	pid_t ps[5];
	int pCount = 5;
	
	// shared memory
	int shmid;
	key_t key;
	
	// file name
	char fname[] = "palindromes.txt";
	
	// the timer information
    time_t endwait;
    time_t start = time(NULL);
    int timeToWait = 5; 		// end loop after this time has elapsed
	
	// for printing errors
	snprintf(errstr, sizeof errstr, "%s: Error: ", argv[0]);
	int i, c;
	opterr = 0;	

	// parse the command line arguments
	while ((c = getopt(argc, argv, ":t:hl::")) != -1) {
		switch (c) {
			// sets the amount of time to let a program run until termination
			case 't': {
				timeToWait = atoi(optarg);
				printf ("Program run time set to: %i\n", timeToWait);
				break;
			}
			// setting name of file of palindromes to read from	
			case 'l': {
				int result;
				char newname[200];
				strcpy(newname, optarg);
				result = rename(fname, newname);
				if(result == 0) {
					printf ("File renamed from %s to %s\n", fname, newname);
					strcpy(fname, newname);
				}
				else {
					perror(errstr);
					printf("Error renaming file.\n");
				}
				break;
			}
			// show help
			case 'h': {
				fprintf(stderr, "\n----------\n");
				fprintf(stderr, "HELP LIST: \n");
				fprintf(stderr, "-t: \n");
				fprintf(stderr, "\tSets the amount of seconds to wait before terminating program. \n");
				fprintf(stderr, "\tDefault is 60 seconds. Must be a number. \n");
				fprintf(stderr, "\tex: -t 60 \n");
				
				fprintf(stderr, "-l: \n");
				fprintf(stderr, "\tSets the name of the file to look for palindromes in. Please include extension.\n");
				fprintf(stderr, "\tex: -l filename \n");
				
				fprintf(stderr, "-h: \n");
				fprintf(stderr, "\tShow help, valid options and required arguments. \n");
				fprintf(stderr, "----------\n\n");
				break;
			}
			// if no argument is given, print an error and end.
			case ':': {
				perror(errstr);
				fprintf(stderr, "-%s requires an argument. \n", optopt);
				return 1;
			}
			// if an invalid option is caught, print that it is invalid and end
			case '?': {
				perror(errstr);
				fprintf(stderr, "Invalid option(s): %s \n", optopt);
				return 1;
			}
		}
	}

	// compute time to end program
    endwait = start + timeToWait;

    printf("Start time is : %s", ctime(&start));
	
	
	// SHARED MEMORY STUFF
	// shared mem is called '1001' which is a palindrome
	key = 1001;
	int lc = count(fname);			// line count of file
	char (*mylist)[lc][200];		// shared memory list
	
	// create segment of appropriate size to hold all the info from file
	if ((shmid = shmget(key, 10000, IPC_CREAT | 0666)) < 0) {
        perror("shmget failed");
        exit(1);
    }
	
	// attach segment to data space
	if ((mylist = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }
			
	(*mylist)[lc][0] = NULL;	// when mylist[n] returns NULL its because you
								// have reached its end

	
	// open file
	FILE * f = fopen(fname, "r");
	if (f == 0)
    {
		perror(errstr);
        printf("Failed to open %s for reading\n", fname);
        return 1;
    }
	
	// write to shared memory
	int w = 0, g = 0;
	char ch;
	while(!feof(f))
	{
		ch = fgetc(f);
		if(ch == '\n')
		{
			(*mylist)[w][g] = '\0';
			w++;
			g = 0;
		}
		else {
			(*mylist)[w][g] = ch;
			g++;
		}
	}
	fclose(f);
	
	for(i = 0; i < pCount; i++){
		
		if ((ps[i] = fork()) < 0) {
			perror(errstr); 
			printf("Fork failed!\n");
			exit(1);
		}
		else if (ps[i] == 0){
			// pass to the execlp, the name of the code to exec
			// the # child it is
			// the line to execute
			// total # of lines in file
			char id[5];
			sprintf(id, "%i", i);
			char xx[5];
			sprintf(xx, "%i", i);
			char ln[5];
			sprintf(ln, "%i", lc);
			execlp("palin", "palin", id, xx, ln, NULL);
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
off_t fsize(char fname[]){
	struct stat st;

    if (stat(fname, &st) == 0){
        return st.st_size;
	}

	perror(errstr); 
    printf("Cannot determine size of %s\n", fname);

    return 1;
}

// get the # of lines in the file
int count(char fname[]){
	FILE *f = fopen(fname, "r");
	int lines = 0;
	int ch = 0;
	
	if (f == 0){
		perror(errstr); 
		printf("Cannot open %s\n", fname);
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