#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    char command[MAX_COMMAND] = "ls -lsa /u/bank";
    char **toks = parse_cmd(command);
    while (*toks != NULL) {
        printf("%s\n",*toks);
        free(*toks);
        toks++;
    }
    if (*toks == NULL)
        printf("NULL\n");
}
