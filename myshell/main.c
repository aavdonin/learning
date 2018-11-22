#include <stdio.h>
#include "defines.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    char **toks = parse_cmd("ls -lsa /u/bank");
    while (*toks != NULL) {
        printf("%s\n",*toks);
    }
}
