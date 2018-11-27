#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "../defines.h"
#include "array_fnc.h"

void send_msg(int qid, int *clients, char *msg);

int main(int argc, char *argv[]) {
    int msqid;
    message msg_in, msg_reg;
    if ((msqid = msgget(MSGQUEUEKEY, IPC_CREAT | 0777)) < 0) {
        perror("msgget");
        return 1;
    }
    int *clients = NULL;
    while (1) {
        memset(msg_reg.mtext,0,MSGSIZE);
        if (msgrcv(msqid, &msg_reg, MSGSIZE, 1, IPC_NOWAIT) < 0) {
            if (errno != ENOMSG)
                perror("msgrcv(reg)");
        }
        else {
            //received new connection
            printf("Client with pid <%s> connected\n", msg_reg.mtext);
            //add client pid to array
            clients = append(clients, atoi(msg_reg.mtext));
            //send message about new client
            char msg[MSGSIZE];
            sprintf(msg, "Client %d joined us!", atoi(msg_reg.mtext));
            send_msg(msqid, clients, msg);
        }

        memset(msg_in.mtext,0,MSGSIZE);
        if (msgrcv(msqid, &msg_in, MSGSIZE, 2, IPC_NOWAIT) < 0) {
            if (errno != ENOMSG)
                perror("msgrcv(in)");
        }
        else {
            //send received message to all registered clients
            send_msg(msqid, clients, msg_in.mtext);
        }
        struct timespec delay;
        delay.tv_sec = 0;
        delay.tv_nsec = 100000000L; //0.1sec
        nanosleep(&delay,NULL);
    }
}

void send_msg(int qid, int *clients, char *msg) {
    int i;
    for (i = arrlen(clients)-1; i >= 0; i--) { //reverse order for unreg possibility
        message msg_out;
        memset(msg_out.mtext,0,MSGSIZE);
        msg_out.mtype = clients[i];
        strcpy(msg_out.mtext, msg);
        if (msgsnd(qid, &msg_out, strlen(msg), IPC_NOWAIT) < 0) {
            perror("msgsnd");
            //del(clients, i); //unregister client
            //todo: register and unregister clients in separate thread
            //declare client dead if there are some messages addressed to him
            //make blocking queue reads in main thread
        }
    }
}
