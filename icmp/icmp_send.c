#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#include "defines.h"
#include "csum.h"
#define RECV_SIZE PACKET_SIZE+20

int icmp_send(int sockfd, struct sockaddr_in *addr, int ttl,
                                        int outtype, int seq) {
    char buf[PACKET_SIZE];
    char recv_buf[RECV_SIZE];
    struct sockaddr_in r_addr;
    int addr_len = sizeof(r_addr);
    struct icmphdr *icmp, *icmp_in;
    struct timespec time_start, time_end;
    long double rtt_msec = 0;
    struct timeval tv_out;
    tv_out.tv_sec = RECV_TIMEOUT;
    tv_out.tv_usec = 0;

    if (setsockopt(sockfd, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
        printf("\nSetting TTL failed!\n");
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
    //filling packet
    memset(&buf, 0, PACKET_SIZE);
    icmp = (struct icmphdr *) buf;
    icmp->type = ICMP_ECHO;
    icmp->un.echo.id = getpid();

    int i;
    for (i = sizeof(*icmp); i < PACKET_SIZE-1; i++)
        buf[i] = i+'0';
    buf[i] = 0;
    icmp->un.echo.sequence = seq;
    icmp->checksum = checksum((unsigned short int*) icmp, PACKET_SIZE);

    clock_gettime(CLOCK_MONOTONIC, &time_start);

    if (sendto(sockfd, buf, PACKET_SIZE, 0,
        (struct sockaddr*)addr, sizeof(*addr)) <= 0) {
        printf("Packet Sending Failed!\n");
        return 0;
    }
    if (recvfrom(sockfd, recv_buf, RECV_SIZE, 0,
        (struct sockaddr*)&r_addr, &addr_len) <= 0) {
        printf("...\n");
        return 0;
    }
    else {
        icmp_in = (struct icmphdr *) (recv_buf+20);
        clock_gettime(CLOCK_MONOTONIC, &time_end);
        long double rtt = (time_end.tv_sec - time_start.tv_sec) * 1000.0 +
            ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
        if (outtype == 1 && icmp_in->type == ICMP_ECHOREPLY) {
            printf("%d bytes from %s: icmp_seq=%d time=%.3Lf ms\n", PACKET_SIZE,
                inet_ntoa(*(struct in_addr*)&r_addr.sin_addr.s_addr),
                icmp_in->un.echo.sequence, rtt);
            return 1;
        }
        else if (outtype == 2) {
            if (icmp_in->type == ICMP_TIME_EXCEEDED) {
                printf("%s: time=%.3Lf ms\n",
                    inet_ntoa(*(struct in_addr*)&r_addr.sin_addr.s_addr), rtt);
            }
            else if (icmp_in->type == ICMP_ECHOREPLY) {
                printf("%s: time=%.3Lf ms\nDestination reached.\n",
                    inet_ntoa(*(struct in_addr*)&r_addr.sin_addr.s_addr), rtt);
                exit(0);
            }
            return 1;
        }
    }
}
