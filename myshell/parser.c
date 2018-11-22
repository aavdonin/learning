#include <string.h>
#include <stdlib.h>
#include "defines.h"

char** parse_cmd(char *cmd) {
    //function to parse given string with strtok to a set of substrings
    //allocates memory for substrings array and returns pointer to it
    char *delim = " ";
    char **token_arr = malloc(sizeof(char*) * MAX_TOKENS);
    char *token = strtok(cmd, delim);
    while (token != NULL) {
        //add token to array
        *(token_arr++) = malloc(strlen(token));
        token = strtok(NULL, delim);
    }
    *token_arr = NULL;
    return token_arr;
}
