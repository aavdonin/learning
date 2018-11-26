#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "../defines.h"
#include "array_fnc.h"

int main() {
    int inpipe;
    int *clients = NULL;
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
                clients = append(clients, atoi(client_pid));
                //send message about new client
                continue;
            }
            //send received message to all registered clients
            int i;
            int client_cnt = arrlen(clients);
            for (i = client_cnt-1; i >= 0; i--) {
                if (clients[i] <= 0) continue;
                char pipefilename[MAXFILENAMELEN];
                sprintf(pipefilename, "client_%d.pipe", clients[i]);
                int outpipe;
                if ((outpipe = open(pipefilename, O_WRONLY)) <= 0) {
                    perror(pipefilename);
                }
                if (write(outpipe, msg, len) <=0) {
                    perror("Error writing to client.pipe");
                    del(clients, i);
                }
                //receive sigpipe and unregister client
            }
            printf("Incomming message (%d): <%s>\n", len, msg);
        }
    }
}
