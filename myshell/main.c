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
        //search for "|" pipe symbol
        int i;
        int pipe_pos; //pipe symbol position
        for (i=0; i<MAX_TOKENS; i++) {
            if (toks[i] && strcmp(toks[i], "|") == 0) {
                pipe_pos = i;
                break;
            }
            else if (i == 0)
                pipe_pos = -1;
        }
        pid_t pid, pid1, pid2;
        int status;
        if (pipe_pos <0) { //no pipes
            if ((pid = fork()) == 0) {
                execvp(toks[0], toks);
                fprintf(stderr,"Command execution failure\n");
                return 1;
            }
            else if (pid < 0) {
                fprintf(stderr,"Unable to create new process\n");
                exit(1);
            }
            waitpid(pid,&status,0);
            printf("Exit status: %d\n", WEXITSTATUS(status));
        }
        else {  //piped
            int conn[2]; pipe(conn);
            if ((pid1 = fork()) == 0) {
                close(conn[0]); //not gonna read
                dup2(conn[1], 1); close(conn[1]); //forward output to pipe
                execvp(toks[0], toks);
                fprintf(stderr,"Command execution failure\n");
                return 1;
            }
            else if (pid1 < 0) {
                fprintf(stderr,"Unable to create new process\n");
                exit(1);
            }
            if ((pid2 = fork()) == 0) {
                close(conn[1]); //not gonna write
                dup2(conn[0], 0); close(conn[0]); //forward input from pipe
                execvp(toks[pipe_pos + 1], toks + pipe_pos + 1);
                fprintf(stderr,"Command execution failure\n");
                return 1;
            }
            else if (pid2 < 0) {
                fprintf(stderr,"Unable to create new process\n");
                exit(1);
            }
            close(conn[0]); //parent isn't going to read
            close(conn[1]); //or write anything
            waitpid(pid1,&status,0);
            waitpid(pid2,NULL,0);
            printf("Exit status: %d\n", WEXITSTATUS(status));
        }
        i = 0;
        while (i < MAX_TOKENS) {
            if (toks[i])
                free(toks[i]);
            i++;
        }
        free(toks);
    }
}
