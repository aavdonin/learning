#include <stdio.h>
#include <arpa/inet.h>
#include "defines.h"

void print_payload(const u_char *payload, u_int len) {
    int i;
    for(i=0; i<len; i++) {
        printf("%02hhx", *(payload + i));
        if ( (i+1)%8 == 0) printf("\n");
        else if ( (i+1)%2 == 0) printf(" ");
    }
}

void print_ether(const struct sniff_ethernet *ethernet) {
    int i = 0;
    printf("Ethernet source : ");
    for (i=0; i<ETHER_ADDR_LEN; i++)
        printf("%02hhx%c", *(ethernet->ether_shost + i),
        (i+1) != ETHER_ADDR_LEN ? ':' : '\n');
    printf("Ethernet dest   : ");
    for (i=0; i<ETHER_ADDR_LEN; i++)
        printf("%02hhx%c", *(ethernet->ether_dhost + i),
        (i+1) != ETHER_ADDR_LEN ? ':' : '\n');
    printf("Ethernet type   : %d\n", ethernet->ether_type);
}

void print_ip(const struct sniff_ip* ip) {
    printf("IP header len: %d\n", IP_HL(ip)*4);
    printf("IP total  len: %d\n", ip->ip_len);
    printf("IP protocol: %d\n", ip->ip_p);
    printf("IP source: %s\n", inet_ntoa(ip->ip_src));
    printf("IP dest: %s\n", inet_ntoa(ip->ip_dst));
}

void print_tcp(const struct sniff_tcp* tcp) {
    printf("TCP source port: %d\n", tcp->th_sport);
    printf("TCP dest port: %d\n", tcp->th_dport);
    printf("TCP header len: %d\n", TH_OFF(tcp)*4);
    printf("TCP sequence number: %u\n", tcp->th_seq);
    if (tcp->th_flags & TH_ACK)
        printf("TCP acknowledgment number: %u\n", tcp->th_ack);
}

void print_udp(const struct sniff_udp* udp) {
    printf("UDP source port: %u\n", udp->ud_sport);
    printf("UDP dest port: %u\n", udp->ud_dport);
    printf("UDP total len: %u\n", udp->ud_len);
}
