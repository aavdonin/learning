#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 7777
#define BACKLOG 4
#define BUFSIZE 1024

int main (int argc, char *argv[]) {
    int sock_tcp; //socket file descriptor
    struct sockaddr_in addr;
    char buf[BUFSIZE];

    sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_tcp < 0) {
        perror("Socket creation error");
        return 1;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_tcp, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("Socket binding error");
        return 1;
    }
    listen(sock_tcp, BACKLOG);

    while (1) {
        int new_sock;
        struct sockaddr_in peer_addr;
        int addr_size = sizeof(peer_addr);
        new_sock = accept(sock_tcp, (struct sockaddr *)&peer_addr, &addr_size);
        printf("Incoming connection: %s:%d\n", inet_ntoa(peer_addr.sin_addr),
        ntohs(peer_addr.sin_port));

        send(new_sock, buf, recv(new_sock, buf, BUFSIZE, 0), 0);
        close(new_sock);
        printf("disconnected\n");
    }

}
