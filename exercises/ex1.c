#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

char *trimwhitespace(const char *string) {

    while(isspace((unsigned char)string[0]))
        string++;
    char *final = strdup(string);
    int length = strlen(final);
    while(length > 0 && isspace((unsigned char)final[length-1]))
        length--;
    final[length] = '\0';
    return final;
}

char * cleanStr(char * str) {
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == '\n') {
			str[i] = ' ';
		}
	}
	return str;
}


int main(int argc, char * argv[]) {
FILE * file;
char * phrase;
int lines = 0;
const char * delim = ".!?";

if (argc == 2) {
	file = fopen(argv[1], "r");
	if (file == NULL) return EXIT_FAILURE;
	
	fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char * text = (char *) malloc(size * sizeof(char));
    
	fread(text, 1, size, file);
	if (strcmp(text, "") == 0) {
		free(text);
		printf("%d\n", lines);
		return 0;
	}
	phrase = strtok(text, delim);
	lines++;
	while (1) {
		phrase = strtok(NULL, delim);
		if (phrase == NULL) break;
		lines++;
	}
	printf("%d\n", lines);
	free(text);
} else if (argc == 3) {
	file = fopen(argv[2], "r");
	
	if (file == NULL) return EXIT_FAILURE;
	
	fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char * text = (char *) malloc(size * sizeof(char));
    
	fread(text, 1, size, file);
	char * copy = strdup(text);
	if (strcmp(text, "") == 0) {
		free(text);
		printf("%d\n", lines);
		return EXIT_SUCCESS;
	}
	phrase = strtok(text, delim);
	lines++;
	printf("[%d] %s%c\n", lines, cleanStr(trimwhitespace(phrase)), copy[phrase - text + strlen(phrase)]);
	while (1) {
		phrase = strtok(NULL, delim);
		if (phrase == NULL) break;
		lines++;
		printf("[%d] %s%c\n", lines, cleanStr(trimwhitespace(phrase)), copy[phrase - text + strlen(phrase)]);
	}
	free(text);
} else {
	printf("usage: %s [-l] file\n", argv[0]);
}

return EXIT_SUCCESS;

}
