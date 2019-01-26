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
