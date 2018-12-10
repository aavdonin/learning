#include <pthread.h>
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
static void *cl_handler(void *arg);

void *tcp_sock(void *arg) {
    int sock_tcp; //socket file descriptor
    struct sockaddr_in addr;

    sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_tcp < 0) {
        perror("TCP Socket creation error");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_tcp, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("TCP Socket binding error");
        exit(1);
    }
    listen(sock_tcp, BACKLOG);

    while (1) {
        pthread_t t_client;
        int status;
        int new_sock;
        struct sockaddr_in peer_addr;
        int addr_size = sizeof(peer_addr);
        new_sock = accept(sock_tcp, (struct sockaddr *)&peer_addr, &addr_size);
        printf("Incoming TCP connection: %s:%d\n",
        inet_ntoa(peer_addr.sin_addr),
        ntohs(peer_addr.sin_port));

        //start thread for connected client
        status = pthread_create(&t_client, NULL, cl_handler, (void *)&new_sock);
        if (status != 0) {
            perror("Unable to create 't_client' thread!\n");
        }
    }
}

static void *cl_handler(void *arg) {
    int *client_socket = (int *)arg;
    char buf[BUFSIZE];
    ssize_t msglen;
    if ((msglen = recv(*client_socket, buf, BUFSIZE, 0)) < 0) {
        perror("TCP recv failed");
        close(*client_socket);
        pthread_exit(NULL);
    }
    if (send(*client_socket, buf, msglen, 0) < 0) {
        perror("TCP send failed");
    }
    close(*client_socket);
    printf("TCP client disconnected\n");
}
