#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#include "defines.h"
#include "icmp_send.h"

int main(int argc, char *argv[]) {
    int sockfd;
    char *ip_addr = malloc(NI_MAXHOST);
    struct hostent *host;
    struct sockaddr_in addr;

    if (argc != 2) {
        printf("Usage: %s <hostname/ip>\n", argv[0]);
        exit(1);
    }
    printf("Resolving DNS...\n");

    if ((host = gethostbyname(argv[1])) == NULL)
    {
        printf("Could not resolve given hostname.\n");
        exit(1);
    }
    strcpy(ip_addr, inet_ntoa(*(struct in_addr *)host->h_addr));

    addr.sin_family = host->h_addrtype;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr  = *(long*)host->h_addr;

    printf("Resolved IP: %s\n", ip_addr);
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd<0)
    {
        printf("\nSocket error\n");
        return 0;
    }
    int i, cnt=0;
    for (i=1; i<=TRC_MAX_TTL; i++) {
        printf("%d.  ", i);
        cnt += icmp_send(sockfd, &addr, i, 2, i); //2 for trace
    }
}
