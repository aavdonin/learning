#include <sys/select.h>
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
#include "cl_handler.h"
#include "accept.h"

int main(int argc, char *argv[]) {
    int sock_tcp, sock_udp; //socket file descriptor
    struct sockaddr_in addr;

    sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_tcp < 0) {
        perror("TCP Socket creation error");
        exit(1);
    }
    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_udp < 0) {
        perror("UDP Socket creation error");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_tcp, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("TCP Socket binding error");
        exit(1);
    }
    addr.sin_port = htons(UDP_PORT);
    if (bind(sock_udp, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("UDP Socket binding error");
        exit(1);
    }
    listen(sock_tcp, BACKLOG);
    //select
    fd_set fd_in, fd_out;
    struct timeval tv;
    int largest_sock = sock_tcp > sock_udp ? sock_tcp : sock_udp;
    tv.tv_sec  = 0;
    tv.tv_usec = 100000; //0.1sec
    
    while(1) {
        FD_ZERO(&fd_in);
        FD_ZERO(&fd_out);
         
        FD_SET(sock_tcp, &fd_in);
        FD_SET(sock_udp, &fd_in);
        int ret = select(largest_sock + 1, &fd_in, &fd_out, NULL, &tv);
        if (ret == -1)
            perror("select");
        else if (ret == 0) {
            continue;
        }
        if (FD_ISSET(sock_tcp, &fd_in)) {
            int new_sock = accept_tcp(sock_tcp);
            if (new_sock < 0) {
                perror("accept_tcp");
            }
            cl_handler((void *)&new_sock);
        }
        if (FD_ISSET(sock_udp, &fd_in)) {
            int new_sock = accept_udp(sock_udp);
            if (new_sock < 0) {
                perror("accept_udp");
            }
            cl_handler((void *)&new_sock);
        }
    }
}
