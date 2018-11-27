#ifndef _DEFINES_H_
#define _DEFINES_H_

#define CHAT_INPUT_HEIGHT 3
#define MSGSIZE 128
#define MAXCLIENTS 32
#define MAXFILENAMELEN 255
#define MSGQUEUEKEY 42

typedef struct {
    long mtype; //1 - reg new client, 2 - client to server, >2 = client pid
    char mtext[MSGSIZE];
} message;

#endif
