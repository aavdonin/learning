#include <pthread.h>
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
#include "cl_handler.h"
#include "accept.h"

int main(int argc, char *argv[]) {
    thread_info_t pool[THREAD_POOL_SIZE];
    int i, status;
    for (i=0; i<THREAD_POOL_SIZE; i++) {
        if (pipe(pool[i].pipe) < 0) {
            perror("pipe creation failed");
        }
        pool[i].cl_cnt = 0;
        status = pthread_create(&(pool[i].tid), NULL, handler_thread, \
        (void *) &(pool[i]));
        if (status != 0) {
            perror("Unable to create 'handler_thread' thread!\n");
            return 1;
        }
        if (pthread_detach(pool[i].tid) != 0) {
            perror("Unable to detach 'handler_thread' thread!\n");
            return 1;
        }
    }
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
    //poll 'em
    struct pollfd socks[2];
    socks[0].fd = sock_tcp;
    socks[0].events = POLLIN;
    socks[1].fd = sock_udp;
    socks[1].events = POLLIN;

    i = 0;
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
            
            while (pool[i].cl_cnt >= MAX_THREAD_CLIENTS)
                if (++i >= THREAD_POOL_SIZE) i -= THREAD_POOL_SIZE;
            if (write(pool[i].pipe[1], &new_sock, sizeof(new_sock)) > 0) {
                if (++i >= THREAD_POOL_SIZE) i -= THREAD_POOL_SIZE;
            } 
            else {
                perror("Failed to write socket descriptor to pipe");
                close(new_sock);
            }
        }
        if (socks[1].revents & POLLIN) {
            socks[1].revents = 0;
            int new_sock = accept_udp(sock_udp);
            if (new_sock < 0) {
                perror("accept_udp");
            }
            
            while (pool[i].cl_cnt >= MAX_THREAD_CLIENTS)
                if (++i >= THREAD_POOL_SIZE) i -= THREAD_POOL_SIZE;
            if (write(pool[i].pipe[1], &new_sock, sizeof(new_sock)) > 0) {
                if (++i >= THREAD_POOL_SIZE) i -= THREAD_POOL_SIZE;
            } 
            else {
                perror("Failed to write socket descriptor to pipe");
                close(new_sock);
            }
        }
    }
}
