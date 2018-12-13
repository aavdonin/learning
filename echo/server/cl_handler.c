#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include "../defines.h"

void *cl_handler(void *arg) {
    int client_socket = *((int *)arg);
    char buf[BUFSIZE];
    ssize_t msglen;
    while(1) {
        if ((msglen = recv(client_socket, buf, BUFSIZE, 0)) < 0) {
            perror("recv failed");
            break;
        }
        else if (msglen == 0) {
            printf("client closed connection\n");
            break;
        }
        if (send(client_socket, buf, msglen, 0) < 0) {
            perror("send failed");
            break;
        }
    }
    close(client_socket);
}
