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

void *udp_sock(void *arg) {
    int sock_udp; //socket file descriptor
    struct sockaddr_in addr;
    char buf[BUFSIZE];

    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_udp < 0) {
        perror("UDP Socket creation error");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_udp, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("UDP Socket binding error");
        exit(1);
    }

    while (1) {
        struct sockaddr_in peer_addr;
        socklen_t addr_size = sizeof(peer_addr);
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

        if (sendto(sock_udp, buf, msglen, MSG_CONFIRM,
        (const struct sockaddr *)&peer_addr, addr_size) < 0) {
            perror("UDP sendto failed");
        }
    }
}
