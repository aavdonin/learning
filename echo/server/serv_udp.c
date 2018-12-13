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
        char buf[BUFSIZE];
        int new_sock; //socket for client handling
        struct sockaddr_in peer_addr, new_sv_addr;
        socklen_t addr_size = sizeof(peer_addr);
        socklen_t new_addr_size = sizeof(new_sv_addr);
        ssize_t msglen;
        msglen = recvfrom(sock_udp, buf, BUFSIZE, MSG_WAITALL,
        (struct sockaddr *)&peer_addr, &addr_size);
        if (msglen < 0) {
            perror("UDP recvfrom failed");
            break;
        }
        buf[msglen] = '\0';
        printf("Incoming UDP connection: %s:%d\n",
        inet_ntoa(peer_addr.sin_addr),
        ntohs(peer_addr.sin_port));
        
        new_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (new_sock < 0) {
            perror("UDP Socket creation error");
            exit(1);
        }
        new_sv_addr.sin_family = AF_INET;
        new_sv_addr.sin_port = 0;   //port=0 to get random port number
        new_sv_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(new_sock, (struct sockaddr*)&new_sv_addr, new_addr_size) != 0){
            perror("UDP Socket binding error (2)");
            close(new_sock);
            continue;
        }
        if (getsockname(new_sock, (struct sockaddr*)&new_sv_addr, 
        &new_addr_size) < 0) {
            perror("Unable to retrieve socket info");
            close(new_sock);
            continue;
        }
        
        if (connect(new_sock, (struct sockaddr*)&peer_addr, addr_size) != 0) {
            perror("UDP Socket connect error");
            return;
        }
        //start thread for connected client
        status = pthread_create(&t_client, NULL, cl_handler, (void *)&new_sock);
        if (status != 0) {
            perror("Unable to create 't_client' thread!\n");
        }
        
        //get new port number into buf
        memset(buf, 0, BUFSIZE);
        sprintf(buf, "%d\n", ntohs(new_sv_addr.sin_port));
        //and send it to client
        if (sendto(sock_udp, buf, strlen(buf), MSG_CONFIRM,
        (const struct sockaddr *)&peer_addr, addr_size) < 0) {
            perror("UDP sendto failed");
        }
    }
}

static void *cl_handler(void *arg) {
    int client_socket = *((int *)arg);
    char buf[BUFSIZE];
    ssize_t msglen;
    while(1) {
        if ((msglen = recv(client_socket, buf, BUFSIZE, 0)) < 0) {
            perror("UDP recv failed");
            break;
        }
        else if (msglen == 0) {
            printf("UDP client closed connection\n");
            break;
        }
        if (send(client_socket, buf, msglen, 0) < 0) {
            perror("UDP send failed");
            break;
        }
    }
    close(client_socket);
}