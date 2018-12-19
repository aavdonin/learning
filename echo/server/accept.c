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

int accept_tcp(int sock_tcp) {
    struct sockaddr_in peer_addr;
    int addr_size = sizeof(peer_addr);
    int new_sock = accept(sock_tcp, (struct sockaddr *)&peer_addr, &addr_size);
    printf("Incoming TCP connection: %s:%d\n", inet_ntoa(peer_addr.sin_addr),
    ntohs(peer_addr.sin_port));
    return new_sock;
}

int accept_udp(int sock_udp) {
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
        return -1;
    }
    buf[msglen] = '\0';
    printf("Incoming UDP connection: %s:%d\n",
    inet_ntoa(peer_addr.sin_addr),
    ntohs(peer_addr.sin_port));
    
    new_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (new_sock < 0) {
        perror("UDP Socket creation error");
        return -1;
    }
    new_sv_addr.sin_family = AF_INET;
    new_sv_addr.sin_port = 0;   //port=0 to get random port number
    new_sv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(new_sock, (struct sockaddr*)&new_sv_addr, 
    new_addr_size) != 0){
        perror("UDP Socket binding error (2)");
        close(new_sock);
        return -1;
    }
    if (getsockname(new_sock, (struct sockaddr*)&new_sv_addr, 
    &new_addr_size) < 0) {
        perror("Unable to retrieve socket info");
        close(new_sock);
        return -1;
    }
    
    if (connect(new_sock, (struct sockaddr*)&peer_addr, 
    addr_size) != 0) {
        perror("UDP Socket connect error");
        close(new_sock);
        return -1;
    }
    //get new port number into buf
    memset(buf, 0, BUFSIZE);
    sprintf(buf, "%d\n", ntohs(new_sv_addr.sin_port));
    //and send it to client
    if (sendto(sock_udp, buf, strlen(buf), MSG_CONFIRM,
    (const struct sockaddr *)&peer_addr, addr_size) < 0) {
        perror("UDP sendto failed");
        close(new_sock);
        return -1;
    }
    return new_sock;
}