#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "trimspaces.h"

//This Function was based in an online solution

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
