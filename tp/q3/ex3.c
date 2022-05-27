#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "wordsProcess.h"

#define READ_END 0
#define WRITE_END 1


int numOfLines(char * filename) {
	FILE * file = fopen(filename, "r");
	
	int res = 0;
	size_t str_size = 0;
	char* str = NULL;
	while ( getline(&str, &str_size, file) > 0 ) {
		if (strlen(str) > 1) res++;
	}
	
	fclose(file);
	return res;	
}


int main(int argc, char* argv[]) {
	
	int input[2];
	int output[2];
	
	if (pipe(input) < 0) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}
	
	if (pipe(output) < 0) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}
	
	int child = fork();
	
	if (child < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (child > 0) {
		//Parent Process
		close(input[READ_END]);
		close(output[WRITE_END]);	

		char ch;
		while(read(STDIN_FILENO, &ch, 1) > 0) {
			if (write(input[WRITE_END], &ch, 1) < 0) {
				fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
			}
		}
		
		close(input[WRITE_END]);
		
		if (waitpid(child, NULL, 0) < 0) {
			fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
		}
		
		while (read(output[READ_END], &ch, 1) > 0)
        {
            write(STDOUT_FILENO, &ch, 1);
        }
		close(output[READ_END]);	
		
		exit(EXIT_SUCCESS);
	
	} else {
		//Child Process
		close(input[WRITE_END]);
		close(output[READ_END]);
		char ch;
		int size = 0;
		char *text;
		while (read(input[READ_END], &ch, 1) > 0) {
			size++;
			text = (char *) realloc(text, size * sizeof(char));
			text[size-1] = ch; 
		}

		close(input[READ_END]);
		
		FILE * cypher = fopen("cypher.txt", "r");
		
		if (cypher == NULL) {
			fprintf(stdout, "No 'cypher.txt' file found. Returning the original string\n\n");
			if (write(output[WRITE_END], text, size) < 0) {
				fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
			}
			exit(EXIT_SUCCESS);
		}
		
		fclose(cypher);
		
		int lines = numOfLines("cypher.txt") * 2;
		
		char * wordsFrom[lines];
		char * wordsTo[lines];
		
		cypher = fopen("cypher.txt", "r");
		char* str = NULL;
		size_t str_size = 0;
		int i = 0;
		while ( getline(&str, &str_size, cypher) > 0 ) {
			if (strlen(str) <= 1) continue;
			
			char * token = strtok(str, " \n");
			
			wordsFrom[i] = malloc(strlen(token) * sizeof(char));
			wordsTo[i + 1] = malloc(strlen(token) * sizeof(char));
			
			strcpy(wordsFrom[i], token);
			strcpy(wordsTo[i + 1], token);
			
			token = strtok(NULL, " \n");
			
			
			wordsTo[i] = malloc(strlen(token) * sizeof(char));
			wordsFrom[i + 1] = malloc(strlen(token) * sizeof(char));
			
			strcpy(wordsTo[i], token);
			strcpy(wordsFrom[i + 1], token);
			i += 2;	
		}
		fclose(cypher);
		
		
		str_size = newSize(text, wordsFrom, wordsTo, lines);
		text = (char *) realloc(text, str_size);
		replaceWord(text, wordsFrom, wordsTo, lines);
			
		for (int i = 0; i < lines; i++) {
			free(wordsFrom[i]);
			free(wordsTo[i]);
		}
		
		
		if (write(output[WRITE_END], text, str_size) < 0) {
			fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
		}
		close(output[WRITE_END]);
		
		exit(EXIT_SUCCESS);
	}
	
	return EXIT_SUCCESS;
}
