#include <sys/epoll.h>
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

#define MAX_EVENTS 2

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
    //poll
    struct epoll_event ev, events[MAX_EVENTS];
    int nfds, epollfd;
    epollfd = epoll_create(1);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(1);
    }
    ev.events = EPOLLIN | EPOLLONESHOT;
    ev.data.fd = sock_tcp;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_tcp, &ev) == -1) {
        perror("epoll_ctl: sock_tcp");
        exit(1);
    }
    ev.data.fd = sock_udp;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_udp, &ev) == -1) {
        perror("epoll_ctl: sock_udp");
        exit(1);
    }

    while(1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENTS, -1); // "-1"=forever
        if (ret == -1)
            perror("epoll_wait");
        else if (ret == 0) {
            continue;
        }
        int i;
        for (i=0; i<MAX_EVENTS; i++) {
            if (events[i].data.fd == sock_tcp) {
                int new_sock = accept_tcp(sock_tcp);
                if (new_sock < 0) {
                    perror("accept_tcp");
                }
                cl_handler((void *)&new_sock);
            }
            else if (events[i].data.fd == sock_udp) {
                int new_sock = accept_udp(sock_udp);
                if (new_sock < 0) {
                    perror("accept_udp");
                }
                cl_handler((void *)&new_sock);
            }
        }
    }
}
