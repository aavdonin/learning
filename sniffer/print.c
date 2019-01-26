#include <stdio.h>
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
