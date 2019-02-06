#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../defines.h"

#define MY_PORT 50000

char* receive(int sock, char *buf, int bsize);

int main(int argc, char *argv[]) {
    int sock_raw; //socket file descriptor
    int new_port; //port number that client will be "reconnected" to
    struct sockaddr_in addr;
    ssize_t msglen;
    int headerlen = 28;
    char buf[BUFSIZE];
    char recvbuf[BUFSIZE];
    char *payload;
    struct udphdr *udp_header_out;
    struct iphdr *ip_header_out;

    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock_raw < 0) {
        perror("RAW Socket creation error");
        exit(1);
    }
    int on = 1;
    if (setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) == -1) {
        perror("setsockopt() failed");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    //say hello
    memset(buf, 0, BUFSIZE);
    ip_header_out = (struct iphdr *) buf;
    ip_header_out->version = 4;
    ip_header_out->ihl = 5;
    ip_header_out->tot_len = htons(headerlen);
    ip_header_out->ttl = 0x40;
    ip_header_out->protocol = 0x11;
    ip_header_out->saddr = inet_addr("127.0.0.1");
    ip_header_out->daddr = inet_addr("127.0.0.1");
    udp_header_out = (struct udphdr *) (buf + 20);
    udp_header_out->uh_sport = htons(MY_PORT);
    udp_header_out->uh_dport = htons(UDP_PORT);
    udp_header_out->uh_ulen = htons(sizeof(udp_header_out));
    udp_header_out->uh_sum = 0;
    if (sendto(sock_raw, buf, ntohs(ip_header_out->tot_len), MSG_CONFIRM,
    (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("UDP sendto failed (hello)");
        exit(1);
    }
    //get port number as an answer
    payload = receive(sock_raw, recvbuf, BUFSIZE);
    new_port = atoi(payload);
    addr.sin_port = htons(new_port);

    while (1) {
        memset(buf, 0, BUFSIZE);
        ip_header_out->version = 4;
        ip_header_out->ihl = 5;
        ip_header_out->ttl = 0x40;
        ip_header_out->protocol = 0x11;
        ip_header_out->saddr = inet_addr("127.0.0.1");
        ip_header_out->daddr = inet_addr("127.0.0.1");
        udp_header_out->uh_sport = htons(MY_PORT);
        udp_header_out->uh_dport = htons(new_port);
        char *msg = buf + headerlen;
        printf("Enter message to send (or \"!quit\"): ");
        fgets(msg, BUFSIZE - headerlen, stdin);
        msg[strlen(msg)-1] = '\0'; //replace '\n' with '\0'
        if (strcmp(msg, "!quit") == 0) {
            //say goodbye
            printf("Disconnecting\n");
            memset(msg, 0, BUFSIZE - headerlen);
            ip_header_out->tot_len = htons(headerlen);
            udp_header_out->uh_ulen = htons(sizeof(udp_header_out));
            if (sendto(sock_raw, buf, headerlen, MSG_CONFIRM,
            (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("UDP sendto failed (bye)");
                exit(1);
            }
            break;
        }
        ip_header_out->tot_len = htons(headerlen + strlen(msg));
        udp_header_out->uh_ulen = htons(sizeof(udp_header_out) + strlen(msg));
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

char* receive(int sock, char *buf, int bsize) {
    char *payload;
    while(1) {
        memset(buf, 0, bsize);
        ssize_t msglen = recv(sock, buf, bsize, 0);
        if (msglen == -1) {
            perror("UDP recv failed (port)");
            exit(1);
        }
        struct iphdr *ip_header_in = (struct iphdr *) buf;
        if (ip_header_in->protocol != 0x11) { //filter only UDP packets
            continue;
        }
        int ihl = (ip_header_in->ihl & 0x0f)*4;
        struct udphdr *udp_header_in = (struct udphdr *) (buf + ihl);
        //filter by port number
        if (ntohs(udp_header_in->uh_dport) != MY_PORT) {
            continue;
        }
        payload = buf + ihl + sizeof(udp_header_in);
        break;
    }
    return payload;
}
