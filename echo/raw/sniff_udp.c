#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define HEADER_SIZE (sizeof(struct iphdr) + sizeof(struct udphdr))
#define BUFSIZE 8196

void print_payload(const u_char *payload, u_int len) {
    int i;
    for(i=0; i<len; i++) {
        printf("%02hhx", *(payload + i));
        if ( (i+1)%8 == 0) printf("\n");
        else if ( (i+1)%2 == 0) printf(" ");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int sock_raw; //socket file descriptor
    struct sockaddr_in addr;
    ssize_t msglen;
    char buf[BUFSIZE];
    char *payload;
    struct udphdr *udp_header_in;
    struct iphdr *ip_header_in;
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock_raw < 0) {
        perror("RAW Socket creation error");
        exit(1);
    }
    while(1) {
        memset(buf, 0, BUFSIZE);
        msglen = recv(sock_raw, buf, BUFSIZE, 0);
        if (msglen == -1) {
            perror("UDP recvfrom failed (port)");
            exit(1);
        }
        ip_header_in = (struct iphdr *) buf;
        if (ip_header_in->protocol != 0x11) {
            printf("Got packet, but it's not UDP\n");
            continue;
        }
        int ihl = (ip_header_in->ihl & 0x0f)*4;
        printf("IP header len: %d\n", ihl);
        printf("IP total len: %d\n", ntohs(ip_header_in->tot_len));
        udp_header_in = (struct udphdr *) (buf + ihl);
        printf("UDP source port: %u\n", ntohs(udp_header_in->uh_sport));
        printf("UDP dest port: %u\n", ntohs(udp_header_in->uh_dport));
        printf("UDP total len: %u\n", ntohs(udp_header_in->uh_ulen));
        printf("UDP checksum: %u\n", ntohs(udp_header_in->uh_sum));

        /*if (ntohs(udp_header_in->uh_dport) != 50000) {
            printf("Got packet, but it's not to my port: %d\n", udp_header_in->uh_dport);
            continue;
        }*/
        payload = buf + ihl + 8;
        printf("Payload: <%s>\n", payload);
        /*new_port = atoi(payload);
        addr.sin_port = htons(new_port);
        printf("got new port: %d\n", new_port);*/
        //break;
        printf("Packet dump:\n");
        print_payload(buf,ntohs(ip_header_in->tot_len));
    }
}
