#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "defines.h"
#include "print.h"

void callback_fnc(u_char *args, const struct pcap_pkthdr *header,
const u_char *packet);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        if (argc < 2)
            fprintf(stderr, "Not enough arguments!\n");
        if (argc > 3)
            fprintf(stderr, "Too much arguments!\n");
        fprintf(stderr, "Usage: %s \"filter expression\" [interface]\n",
        argv[0]);
        exit(1);
    }
    char *dev, errbuf[PCAP_ERRBUF_SIZE];
    if (argc == 3)
        dev = argv[2];
    else {
        dev = pcap_lookupdev(errbuf);
        if (dev == NULL) {
                fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
                return(2);
        }
    }
    printf("Device: %s\n", dev);

    pcap_t *handle;
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
         fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
         return(2);
    }
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "Device %s doesn't provide Ethernet headers" \
        " - not supported\n", dev);
        return(2);
    }
    struct bpf_program fp;		/* The compiled filter expression */
    char *filter_exp = argv[1];	/* The filter expression */
    bpf_u_int32 mask;		/* The netmask of sniffing device */
    bpf_u_int32 net;		/* The IP of sniffing device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", dev);
        net = 0;
        mask = 0;
    }
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
        filter_exp, pcap_geterr(handle));
        return(2);
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
        filter_exp, pcap_geterr(handle));
        return(2);
    }
    pcap_loop(handle, -1, callback_fnc, NULL);
    pcap_close(handle);
    return(0);
}

void callback_fnc(u_char *args, const struct pcap_pkthdr *header,
const u_char *packet) {
    /* ethernet headers are always exactly 14 bytes */
    #define SIZE_ETHERNET 14
    const struct sniff_ethernet *ethernet; /* The ethernet header */
    const struct sniff_ip *ip; /* The IP header */
    const struct sniff_tcp *tcp; /* The TCP header */
    const struct sniff_udp *udp; /* The UDP header */
    const char *payload; /* Packet payload */

    u_int size_ip;
    u_int size_tcpudp;
    ethernet = (struct sniff_ethernet*)(packet);
    print_ether(ethernet);

    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }
    print_ip(ip);

    if (ip->ip_p == 0x6) {  //TCP
        tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
        size_tcpudp = TH_OFF(tcp)*4;
        if (size_tcpudp < 20) {
            printf("   * Invalid TCP header length: %u bytes\n", size_tcpudp);
            return;
        }
        print_tcp(tcp);
    }
    else if (ip->ip_p == 0x11) {    //UDP
        udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
        size_tcpudp = 8;
        print_udp(udp);
    }

    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcpudp);
    int payload_len = header->caplen - (SIZE_ETHERNET + size_ip + size_tcpudp);
    printf("Payload:\n");
    print_payload(payload, payload_len);
    printf("\n--------------------\n");
}
