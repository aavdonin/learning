#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "../defines.h"


int main() {
    int inpipe;
    int client_pids[MAXCLIENTS];
    if (mkfifo("server.pipe", 0777)) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }
    }
    if ((inpipe = open("server.pipe", O_RDONLY|O_NDELAY)) <= 0) {
        perror("open server.pipe");
        return 1;
    }
    int len;
    char msg[MSGSIZE];
    while (1) {
        if ((len = read(inpipe, msg, MSGSIZE)) > 0) {
            msg[len] = '\0';
            if (msg[0] == '!' && strncmp(msg, "!connect", 8) == 0) {
                //received "!connect" command
                char client_pid[8];
                strcpy(client_pid, msg+9);
                printf("Client with pid <%s> connected\n", client_pid);
                //add client pid to array
                //send message about new client
                continue;
            }
            //send received message to all registered clients
            //receive sigpipe and unregister client
            printf("Incomming message (%d): <%s>\n", len, msg);
        }
    }
}