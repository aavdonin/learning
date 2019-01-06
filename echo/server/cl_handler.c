#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

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

void *handler_thread(void *arg) {
    thread_info_t *thread_info = (thread_info_t *) arg;
    struct pollfd fds[MAX_THREAD_CLIENTS+1];
    int i;
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
        if (fds[0].revents & POLLIN) {//received new socket descriptor from pipe
            fds[0].revents = 0;
            int new_sock;
            if (read(fds[0].fd, &new_sock, sizeof(new_sock)) <= 0) {
                perror("thread read from pipe err");
            }
            else {
                //add that socket into watch list
                for (i=1; i<=MAX_THREAD_CLIENTS; i++) {
                    if (fds[i].fd < 0) {
                        fds[i].fd = new_sock;
                        fds[i].events = POLLIN;
                        ++thread_info->cl_cnt;
                        break;
                    }
                }
            }
        }

        for (i=1; i<=MAX_THREAD_CLIENTS; i++) {
            if (fds[i].revents & POLLIN) {
                //read and answer to socket
                char buf[BUFSIZE];
                ssize_t msglen;
                if ((msglen = recv(fds[i].fd, buf, BUFSIZE, 0)) < 0) {
                    perror("recv failed");
                    continue;
                }
                else if (msglen == 0) {
                    printf("client closed connection\n");
                    close(fds[i].fd);
                    fds[i].fd *= -1; //disable listening on this fd
                    --thread_info->cl_cnt;
                    continue;
                }
                if (send(fds[i].fd, buf, msglen, 0) < 0) {
                    perror("send failed");
                    continue;
                }
            }
        }
    }
}
