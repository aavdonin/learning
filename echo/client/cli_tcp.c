#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "../defines.h"

void tcp_client(void) {
    int sock_tcp; //socket file descriptor
    struct sockaddr_in addr;
    char buf[BUFSIZE];
    char recvbuf[BUFSIZE];
    ssize_t msglen;

    sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_tcp < 0) {
        perror("TCP Socket creation error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (connect(sock_tcp, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("TCP connect error");
        return;
    }
    while(1) {
        printf("Enter message to send (or \"!quit\"): ");
        fgets(buf, BUFSIZE, stdin);
        buf[strlen(buf)-1] = '\0'; //replace '\n' with '\0'
        if (strcmp(buf, "!quit") == 0)
            break;

        if (send(sock_tcp, buf, strlen(buf), 0) < 0) {
            perror("TCP send failed");
            return;
        }
        printf("Sent:     <%s>\n", buf);
        memset(recvbuf, 0, BUFSIZE);
        if ((msglen = recv(sock_tcp, recvbuf, BUFSIZE, 0)) < 0) {
            perror("TCP recv failed");
            return;
        }
        printf("Received: <%s>\n", recvbuf);
    }
    close(sock_tcp);
}
