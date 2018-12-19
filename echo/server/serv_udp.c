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
#include "cl_handler.h"
#include "accept.h"

void *udp_sock(void *arg) {
    int sock_udp; //socket that listens to new "connections"
    struct sockaddr_in sv_addr;

    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_udp < 0) {
        perror("UDP Socket creation error");
        exit(1);
    }

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(UDP_PORT);
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_udp, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) != 0) {
        perror("UDP Socket binding error");
        exit(1);
    }

    while (1) {
        pthread_t t_client;
        int status;
        int new_sock = accept_udp(sock_udp);
        //start thread for connected client
        status = pthread_create(&t_client, NULL, cl_handler, (void *)&new_sock);
        if (status != 0) {
            perror("Unable to create 't_client' thread!\n");
            close(new_sock);
            continue;
        }
        if (pthread_detach(t_client) != 0) {
            perror("Unable to detach 't_client' thread!\n");
        }
    }
}
