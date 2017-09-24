#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("I'm a child process!!\n");
	fprintf(stderr, "Process ID: %ld\n", (long)getpid());
	fprintf(stderr, "Parent ID: %ld\n\n", (long)getppid());
	return 0;
}