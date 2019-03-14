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

void print_payload(const u_char *payload, u_int len) {
    int i;
    for(i=0; i<len; i++) {
        printf("%02hhx", *(payload + i));
        if ( (i+1)%8 == 0) printf("\n");
        else if ( (i+1)%2 == 0) printf(" ");
    }
    printf("\n");
}

int icmp_send(int sockfd, struct sockaddr_in *addr, int ttl, int outtype, int seq) {
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
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, /*(const char*)*/&tv_out, sizeof(tv_out));
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

    if (sendto(sockfd, buf, PACKET_SIZE, 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0) {
        printf("Packet Sending Failed!\n");
        return 0;
    }
    if (recvfrom(sockfd, recv_buf, RECV_SIZE, 0, (struct sockaddr*)&r_addr, &addr_len) <= 0) {
        printf("...\n");
        //no return if timeout leads here
    }
    else {
        icmp_in = (struct icmphdr *) (recv_buf+20);
        clock_gettime(CLOCK_MONOTONIC, &time_end);
        long double rtt = (time_end.tv_sec - time_start.tv_sec) * 1000.0 +
            ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
        printf("%d bytes from %s: icmp_seq=%d time=%.3Lf ms\n", PACKET_SIZE,
            inet_ntoa(*(struct in_addr*)&r_addr.sin_addr.s_addr), icmp_in->un.echo.sequence, rtt);
        //print_payload(recv_buf,PACKET_SIZE);
        return 1;
    }
    return 0;
}
