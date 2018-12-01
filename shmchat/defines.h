#ifndef _DEFINES_H_
#define _DEFINES_H_

#define CHAT_INPUT_HEIGHT 3
#define MSGSIZE 128
#define MA 0
#define RD 1
#define CA 2

typedef struct {
    int client_num;
    char text[MSGSIZE];
} mem_t;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#endif
