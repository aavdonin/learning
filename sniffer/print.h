#ifndef _PRINT_H_
#define _PRINT_H_

#include <netinet/in.h>

void print_payload(const u_char *payload, u_int len);
void print_ether(const struct sniff_ethernet *ethernet);
#endif
