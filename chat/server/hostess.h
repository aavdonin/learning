#ifndef _HOSTESS_H_
#define _HOSTESS_H_

void send_msg(int qid, int *clients, char *msg);
void *hostess(void *arg);

#endif
