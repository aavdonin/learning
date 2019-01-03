#include <poll.h>
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
//#include "cl_handler.h"
#include "accept.h"

typedef struct {
    pthread_t tid;
    int pipe[2];
    int cl_cnt;
} thread_info_t;
#define MAX_THREAD_CLIENTS 100
void *handler_thread(void *arg);

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
    struct pollfd socks[2];
    socks[0].fd = sock_tcp;
    socks[0].events = POLLIN;
    socks[1].fd = sock_udp;
    socks[1].events = POLLIN;

    while(1) {
        int ret = poll(socks, (nfds_t)2, 100); //100 = 0.1sec
        if (ret == -1)
            perror("poll");
        else if (ret == 0) {
            continue;
        }
        if (socks[0].revents & POLLIN) {
            int new_sock = accept_tcp(sock_tcp);
            socks[0].revents = 0;
            if (new_sock < 0) {
                perror("accept_tcp");
            }
            //cl_handler((void *)&new_sock);
        }
        if (socks[1].revents & POLLIN) {
            socks[1].revents = 0;
            int new_sock = accept_udp(sock_udp);
            if (new_sock < 0) {
                perror("accept_udp");
            }
            //cl_handler((void *)&new_sock);
        }
    }
}

void *handler_thread(void *arg) {
    thread_info_t *thread_info = (thread_info_t *) arg;
    struct pollfd fds[MAX_THREAD_CLIENTS+1];
    fds[0].fd = thread_info->pipe[0]; //will listen to pipe's output
    fds[0].events = POLLIN;
    for (i=1; i<=MAX_THREAD_CLIENTS; i++)
        fds[i].fd = -1; //initialize other file descriptors
    while(1) {
        int ret = poll(fds, MAX_THREAD_CLIENTS+1, -1); //-1 for infinite wait
        if (ret == -1)
            perror("poll");
        else if (ret == 0) {
            continue;
        }
        if (fds[0].revents & POLLIN) {
            fds[0].revents = 0;
            int new_sock;
            if (read(fds[0], new_sock, sizeof(new_sock)) <= 0) {
                perror("thread read from pipe err");
                continue;
            }
            fds[++thread_info->cl_cnt].fd = new_sock; //don't rely on cl_cnt here
            fds[thread_info->cl_cnt].events = POLLIN; //it's wrong after disconnecting a client
        }
        int i;
        for (i=1; i<=MAX_THREAD_CLIENTS; i++) {
            if (fds[i].revents & POLLIN) {
                //read and answer to socket
            }
        }
    }
}
