#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "defines.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    char command[MAX_COMMAND];
    while (1) {
        printf("myshell> "); fgets(command, MAX_COMMAND, stdin); //get command
        if (strcmp(command, "quit\n") == 0) return 0;
        char **toks = parse_cmd(command);
        pid_t pid;
        int status;
        switch (pid = fork()) {
        case -1:
            fprintf(stderr,"Unable to create new process\n");
            exit(1);
        case 0:
            execvp(toks[0], toks);
            fprintf(stderr,"Command execution failure\n");
            return 1;
        default:
            waitpid(pid,&status,0);
            printf("Exit status: %d\n", WEXITSTATUS(status));
            int i = 0;
            while (toks[i] != NULL)
                    free(toks[i++]);
            free(toks);
            break;
        }
    }
}
