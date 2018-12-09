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

void udp_client(void) {
    int sock_udp; //socket file descriptor
    struct sockaddr_in addr;
    ssize_t msglen;
    char buf[BUFSIZE];
    char recvbuf[BUFSIZE];

    printf("Enter message to send: ");
    fgets(buf, BUFSIZE, stdin);
    buf[strlen(buf)-1] = '\0'; //replace '\n' with '\0'

    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_udp < 0) {
        perror("UDP Socket creation error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (sendto(sock_udp, buf, strlen(buf), MSG_CONFIRM,
    (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("UDP sendto failed");
        return;
    }
    printf("Sent:     <%s>\n", buf);

    memset(recvbuf, 0, BUFSIZE);
    socklen_t addr_size = sizeof(addr);
    msglen = recvfrom(sock_udp, recvbuf, BUFSIZE, MSG_WAITALL,
    (struct sockaddr *)&addr, &addr_size);
    if (msglen < 0) {
        perror("UDP recvfrom failed");
        return;
    }
    buf[msglen] = '\0';
    printf("Received: <%s>\n", recvbuf);
}
