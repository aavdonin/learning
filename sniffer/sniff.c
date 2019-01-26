#include <pcap.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "defines.h"
#include "print.h"
/*
TODO:
- specify interface and filter expression as command line arguments
- separate funcs for printing packet headers:
 + ethernet
 + ip
 + tcp
 + udp
+ payload hex-printing function
- add UDP
*/
void callback_fnc(u_char *args, const struct pcap_pkthdr *header,
const u_char *packet);

int main(int argc, char *argv[]) {
    char *dev, errbuf[PCAP_ERRBUF_SIZE];

    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
            fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
            return(2);
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
    char filter_exp[] = "port 80";	/* The filter expression */
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
    const char *payload; /* Packet payload */

    u_int size_ip;
    u_int size_tcp;
    ethernet = (struct sniff_ethernet*)(packet);
    print_ether(ethernet);

    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }
    print_ip(ip);

    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }
    print_tcp(tcp);

    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
    int payload_len = header->caplen - (SIZE_ETHERNET + size_ip + size_tcp);
    printf("Payload:\n");
    print_payload(payload, payload_len);
    printf("\n--------------------\n");
}
