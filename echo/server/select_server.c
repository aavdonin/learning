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
    tv.tv_sec = 10; //10sec
    tv.tv_usec = 0; //100000; //0.1sec
    
    FD_ZERO(&fd_in);
    FD_ZERO(&fd_out);
     
    FD_SET(sock_tcp, &fd_in);
    FD_SET(sock_udp, &fd_in);
    int ret = select(largest_sock + 1, &fd_in, &fd_out, NULL, &tv);
    if (ret == -1)
        perror("select");
    else if (ret == 0) {
        //timeout
    }
    else {
        if (FD_ISSET(sock_tcp, &fd_in)) {
            int new_sock;
            struct sockaddr_in peer_addr;
            int addr_size = sizeof(peer_addr);
            new_sock = accept(sock_tcp, (struct sockaddr *)&peer_addr, &addr_size);
            printf("Incoming TCP connection: %s:%d\n",
            inet_ntoa(peer_addr.sin_addr),
            ntohs(peer_addr.sin_port));
            cl_handler((void *)&new_sock);
        }
        if (FD_ISSET(sock_udp, &fd_in)) {
            char buf[BUFSIZE];
            int new_sock;
            struct sockaddr_in peer_addr, new_sv_addr;
            socklen_t addr_size = sizeof(peer_addr);
            socklen_t new_addr_size = sizeof(new_sv_addr);
            ssize_t msglen;
            msglen = recvfrom(sock_udp, buf, BUFSIZE, MSG_WAITALL,
            (struct sockaddr *)&peer_addr, &addr_size);
            if (msglen < 0) {
                perror("UDP recvfrom failed");
                //continue;
            }
            buf[msglen] = '\0';
            printf("Incoming UDP connection: %s:%d\n",
            inet_ntoa(peer_addr.sin_addr),
            ntohs(peer_addr.sin_port));
            
            new_sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (new_sock < 0) {
                perror("UDP Socket creation error");
                //continue;
            }
            new_sv_addr.sin_family = AF_INET;
            new_sv_addr.sin_port = 0;   //port=0 to get random port number
            new_sv_addr.sin_addr.s_addr = INADDR_ANY;
            if (bind(new_sock, (struct sockaddr*)&new_sv_addr, 
            new_addr_size) != 0){
                perror("UDP Socket binding error (2)");
                close(new_sock);
                //continue;
            }
            if (getsockname(new_sock, (struct sockaddr*)&new_sv_addr, 
            &new_addr_size) < 0) {
                perror("Unable to retrieve socket info");
                close(new_sock);
                //continue;
            }
            
            if (connect(new_sock, (struct sockaddr*)&peer_addr, 
            addr_size) != 0) {
                perror("UDP Socket connect error");
                close(new_sock);
                //continue;
            }
            //get new port number into buf
            memset(buf, 0, BUFSIZE);
            sprintf(buf, "%d\n", ntohs(new_sv_addr.sin_port));
            //and send it to client
            if (sendto(sock_udp, buf, strlen(buf), MSG_CONFIRM,
            (const struct sockaddr *)&peer_addr, addr_size) < 0) {
                perror("UDP sendto failed");
                close(new_sock);
                //continue;
            }
            cl_handler((void *)&new_sock);
        }

    }
}
