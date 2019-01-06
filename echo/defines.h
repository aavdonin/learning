#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <sys/types.h>

#define TCP_PORT 7777
#define UDP_PORT 7777

#define BACKLOG 4
#define BUFSIZE 1024
#define CMDSIZE 64

#define MAX_THREAD_CLIENTS 100
#define THREAD_POOL_SIZE 10

typedef struct {
    pthread_t tid;
    int pipe[2];
    int cl_cnt;
} thread_info_t;

#endif
