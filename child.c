#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	pid_t pid = (long)getpid();
	int pNum = atoi(argv[1]);
	int cLine = 0;
	int lineToTest = pNum;
	
	printf("I'm child process #%i!\n", pNum+1);
	fprintf(stderr, "Process ID: %ld\n", pid);
	fprintf(stderr, "Parent ID: %ld\n\n", (long)getppid());
	printf("--------------\n");
	
	FILE * f = fopen("palindromes.txt", "r");
	
	int ch = 0;
	if(f != NULL){
		char testLine[150];
		while (fgets(testLine, sizeof testLine, f) != NULL){
			if (cLine == lineToTest){
				if(is_palindrome(testLine, sizeof testLine) != 0){
					printf("%ld || %s Y!\n", pid, testLine);
					lineToTest = lineToTest + atoi(argv[2]);
				}
				else { printf("%ld || %s N!\n", pid, testLine); 
				}
			}
			else { printf("cline: %i\n", cLine); cLine++; }
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
		if(x == '\0'){ break; }
		else if(isspace(x)){ continue; }
		else if(ispunct(x)){ continue; }
		else if (isalpha(x)){
			newStr[c] = tolower(x);
			c++;
		}
		else { newStr[c] = x; c++; }
	}
	c++;
	newStr[c] = '\0';
	
	printf("Testing: ");
	length = sizeof newStr;
	for(i = 0; i < length; i++){
		char x = newStr[i];
		if(x == '\0'){ break; }
		printf("%c", x);
	}
	printf("\n");
	
    for(i = 0; i < length; i++){
        if(newStr[i] != newStr[length - i - 1]){
            return 0;
	   }
	}
	
    return 1;
}