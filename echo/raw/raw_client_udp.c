#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

#include "../defines.h"
#include "csum.h"
#include "recv_raw.h"

int main(int argc, char *argv[]) {
    int sock_raw; //socket file descriptor
    int new_port; //port number that client will be "reconnected" to
    struct sockaddr_ll addr;
    int ehl = sizeof(struct ether_header);  //eth header len
    int ihl = sizeof(struct iphdr);         //ip  header len
    int uhl = sizeof(struct udphdr);        //udp header len
    int headerlen = ehl + ihl + uhl;
    char buf[BUFSIZE];
    char recvbuf[BUFSIZE];
    char *payload;
    struct ether_header *eth_header_out;
    struct udphdr *udp_header_out;
    struct iphdr *ip_header_out;

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("RAW Socket creation error");
        exit(1);
    }
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(ETH_P_IP);
    addr.sll_ifindex = if_nametoindex("enp0s8");
    addr.sll_halen = ETH_ALEN;

    char mac_dst[ETH_ALEN] = {0x08, 0x00, 0x27, 0x89, 0x34, 0x25};
    char mac_src[ETH_ALEN] = {0x08, 0x00, 0x27, 0x3e, 0x56, 0xeb};
    memcpy(addr.sll_addr, mac_dst, ETH_ALEN);

    //say hello
    memset(buf, 0, BUFSIZE);
    eth_header_out = (struct ether_header *) buf;
    memcpy(eth_header_out->ether_dhost, mac_dst, ETH_ALEN);
    memcpy(eth_header_out->ether_shost, mac_src, ETH_ALEN);
    eth_header_out->ether_type = htons(ETH_P_IP);
    ip_header_out = (struct iphdr *) (buf + ehl);
    ip_header_out->version = 4;
    ip_header_out->ihl = 5;
    ip_header_out->tot_len = htons(headerlen - ehl);
    ip_header_out->ttl = 0x40;
    ip_header_out->protocol = 0x11;
    ip_header_out->saddr = inet_addr(SRC_IP);
    ip_header_out->daddr = inet_addr(DEST_IP);
    ip_header_out->check = checksum((unsigned short int*) ip_header_out, ihl);
    udp_header_out = (struct udphdr *) (buf + ehl + ihl);
    udp_header_out->uh_sport = htons(RAW_PORT);
    udp_header_out->uh_dport = htons(UDP_PORT);
    udp_header_out->uh_ulen = htons(uhl);
    udp_header_out->uh_sum = 0;
    if (sendto(sock_raw, buf, headerlen, 0,
    (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("UDP sendto failed (hello)");
        exit(1);
    }
    //get port number as an answer
    payload = receive(sock_raw, recvbuf, BUFSIZE);
    new_port = atoi(payload);
    //addr.sin_port = htons(new_port);

    while (1) {
        memset(buf, 0, BUFSIZE);
        memcpy(eth_header_out->ether_dhost, mac_dst, ETH_ALEN);
        memcpy(eth_header_out->ether_shost, mac_src, ETH_ALEN);
        eth_header_out->ether_type = htons(ETH_P_IP);
        ip_header_out->version = 4;
        ip_header_out->ihl = 5;
        ip_header_out->ttl = 0x40;
        ip_header_out->protocol = 0x11;
        ip_header_out->saddr = inet_addr(SRC_IP);
        ip_header_out->daddr = inet_addr(DEST_IP);
        udp_header_out->uh_sport = htons(RAW_PORT);
        udp_header_out->uh_dport = htons(new_port);
        char *msg = buf + headerlen;
        printf("Enter message to send (or \"!quit\"): ");
        fgets(msg, BUFSIZE - headerlen, stdin);
        msg[strlen(msg)-1] = '\0'; //replace '\n' with '\0'
        if (strcmp(msg, "!quit") == 0) {
            //say goodbye
            printf("Disconnecting\n");
            memset(msg, 0, BUFSIZE - headerlen);
            ip_header_out->tot_len = htons(headerlen - ehl);
            udp_header_out->uh_ulen = htons(uhl);
            ip_header_out->check = checksum((unsigned short int*) ip_header_out,
            ihl);
            if (sendto(sock_raw, buf, headerlen, MSG_CONFIRM,
            (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("UDP sendto failed (bye)");
                exit(1);
            }
            break;
        }
        ip_header_out->tot_len = htons(headerlen + strlen(msg) - ehl);
        udp_header_out->uh_ulen = htons(uhl + strlen(msg));
        ip_header_out->check = checksum((unsigned short int*)ip_header_out,ihl);
        if (sendto(sock_raw, buf, headerlen + strlen(msg),
        MSG_CONFIRM, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("UDP sendto failed");
            exit(1);
        }
        printf("Sent:     <%s>\n", msg);
        payload = receive(sock_raw, recvbuf, BUFSIZE);
        printf("Received: <%s>\n", payload);
    }
}
