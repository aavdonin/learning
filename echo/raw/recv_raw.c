#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include "../defines.h"

char* receive(int sock, char *buf, int bsize) {
    char *payload;
    while(1) {
        memset(buf, 0, bsize);
        ssize_t msglen = recv(sock, buf, bsize, 0);
        if (msglen == -1) {
            perror("UDP recv failed");
            exit(1);
        }
        int ehl = sizeof(struct ether_header);
        struct ether_header *eth_header_in = (struct ether_header *) buf;
        struct iphdr *ip_header_in = (struct iphdr *) (buf + ehl);
        if (ip_header_in->protocol != 0x11) { //filter only UDP packets
            continue;
        }
        if (ip_header_in->daddr != inet_addr(SRC_IP)) { //filter by IP
            continue;
        }
        int ihl = (ip_header_in->ihl & 0x0f)*4;
        struct udphdr *udp_header_in = (struct udphdr *) (buf + ehl + ihl);
        //filter by port number
        if (ntohs(udp_header_in->uh_dport) != RAW_PORT) {
            continue;
        }
        payload = buf + ehl + ihl + sizeof(udp_header_in);
        break;
    }
    return payload;
}
