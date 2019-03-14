#ifndef _ICMP_SEND_H_
#define _ICMP_SEND_H_

int icmp_send(int sockfd, struct sockaddr_in *addr, int ttl, int outtype, int seq);
#endif

