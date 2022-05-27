#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wordsProcess.h"

//These functions are based in an online solution (function to replace words)
//That we divided to know the size first ands then replace the chars

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
