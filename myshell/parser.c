#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"

static void check_malloc(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr,"Memory allocation failure");
        exit(1);
    }
}

char** parse_cmd(char *cmd) {
    //function to parse given string with strtok to a set of substrings
    //allocates memory for substrings array and returns pointer to it
    const char delim[3] = " \n";
    char **token_arr = malloc(sizeof(char *) * MAX_TOKENS);
    check_malloc(token_arr);
    char *token;
    token = strtok(cmd, delim);
    int i = 0;
    while (token != NULL) {
        //insert NULL before "|" token
        if (strcmp(token, "|") == 0)
            token_arr[i++] = NULL;
        //add token to array
        token_arr[i] = malloc(strlen(token));
        check_malloc(token_arr[i]);
        strcpy(token_arr[i], token);
        token = strtok(NULL, delim);
        i++;
    }
    token_arr[i] = NULL;
    return token_arr;
}
