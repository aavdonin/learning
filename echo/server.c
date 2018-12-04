#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

#define PORT 7777
#define BACKLOG 4
#define BUFSIZE 1024

int main (int argc, char *argv[]) {
    struct sockaddr_storage peer_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sock_tcp; //socket file descriptor
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // automatically fill IP
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }
    sock_tcp = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_tcp < 0) {
        perror("Socket creation error");
        return 1;
    }
    
    if (bind(sock_tcp, res->ai_addr, res->ai_addrlen) != 0) {
        perror("Socket binding error");
        return 1;
    }
    listen(sock_tcp, BACKLOG);

    addr_size = sizeof(peer_addr);
    new_fd = accept(sock_tcp, (struct sockaddr *)&peer_addr, &addr_size);
    
    //receive message
    //send back
    //close socket
}