#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define READ_END 0
#define WRITE_END 1


ssize_t fdReadySize(int fd) { 
    int size;
    int response;

    response = ioctl(fd, FIONREAD, &size);
    if (response) {
        return response;
    }
    return size;
}

int numOfLines(char * filename) {
	FILE * file = fopen(filename, "r");
	int res = 0;
	size_t str_size = 0;
	char* str = NULL;
	while ( getline(&str, &str_size, file) > 0 ) {
		res++;
	}
	fclose(file);
	return res;	
}

void replaceWord(char* str, char* oldWords[],
                  char* newWords[], int n)
{
	char * beginStr = str;
    char* result;
    int newSz = 0;
	
	for (int i = 0; i < n; i++) {
		
		int count = 0;
 
		int newWordlen = strlen(newWords[i]);
		int oldWordlen = strlen(oldWords[i]);
		
	  
		for (int j = 0; str[j] != '\0'; j++) {
			if (strstr(&str[j], oldWords[i]) == &str[j]) {
				count++;
				
				j += oldWordlen - 1;
			}
		}
  
		newSz += count * (newWordlen - oldWordlen);
		
	}
  
    result = (char*)malloc(strlen(str) + newSz + 1);
  
    int i = 0;
    
    while (*str) {
		int found = 0;
		for (int j = 0; j < n; j++) {
			int newWordlen = strlen(newWords[j]);
			int oldWordlen = strlen(oldWords[j]);	
			if (strstr(str, oldWords[j]) == str) {
				strcpy(&result[i], newWords[j]);
				i += newWordlen;
				str += oldWordlen;
				found = 1;
				break;
			}
		}
		if (!found){
			result[i++] = *str++;
		}
        
    }
  
    strcpy(beginStr, result);
    free(result);
}

int newSize(const char* str, char* oldWords[],
                  char* newWords[], int n)
{
	int newSz = 0;
	
	for (int i = 0; i < n; i++) {
		
		int count = 0;
 
		int newWordlen = strlen(newWords[i]);
		int oldWordlen = strlen(oldWords[i]);
		
	  
		for (int j = 0; str[j] != '\0'; j++) {
			if (strstr(&str[j], oldWords[i]) == &str[j]) {
				count++;
				
				j += oldWordlen - 1;
			}
		}
  
		newSz += count * (newWordlen - oldWordlen);
		
	}
	
	return strlen(str) + newSz;
    
}


int main(int argc, char* argv[]) {
	
	int input[2];
	int output[2];
	ssize_t inputSize = fdReadySize(STDIN_FILENO);
	char *text = malloc(inputSize * sizeof(char));
	
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
		int i = 0;
		while(read(STDIN_FILENO, &ch, 1) > 0) {
			text[i] = ch;
			i++;
		}
		text[i - 1] = '\0';
		
		if (write(input[WRITE_END], text, inputSize) < 0) {
			fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
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
		
		if (read(input[READ_END], text, inputSize) < 0 ) {
			fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
		}
		close(input[READ_END]);
		
		int lines = numOfLines("cypher.txt") * 2;
		
		char * wordsFrom[lines];
		char * wordsTo[lines];
		
		FILE * cypher = fopen("cypher.txt", "r");
		char* str = NULL;
		size_t str_size = 0;
		int i = 0;
		while ( getline(&str, &str_size, cypher) > 0 ) {
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
		text = realloc(text, str_size);
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
