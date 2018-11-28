#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../defines.h"
#include "array_fnc.h"

extern int* clients;

void send_msg(int qid, int *clients, char *msg) {
    int i;
    for (i = 0; i < arrlen(clients); i++) {
        message msg_out;
        memset(msg_out.mtext,0,MSGSIZE);
        msg_out.mtype = clients[i];
        strcpy(msg_out.mtext, msg);
        if (msgsnd(qid, &msg_out, strlen(msg), IPC_NOWAIT) < 0) {
            perror("msgsnd");
        }
    }
}

void *hostess(void *arg) {
    int *qid = (int*) arg;
    message msg_reg;
    memset(msg_reg.mtext,0,MSGSIZE);
    while (1) {
        if (msgrcv(*qid, &msg_reg, MSGSIZE, 1, 0) < 0) {
            if (errno != ENOMSG)
                perror("msgrcv(reg)");
        }
        else {
            char msg[MSGSIZE];
            int cl_pid = atoi(msg_reg.mtext);
            if (cl_pid > 0) {
                //received new connection
                printf("Client pid <%d> connected\n", cl_pid);
                //add client pid to array
                clients = append(clients, cl_pid);
                //send message about new client
                sprintf(msg, "User %d joined us!", cl_pid);
            }
            else {
                //client disconnected
                printf("Client pid <%d> disconnected\n", -cl_pid);
                del(clients, -cl_pid);
                sprintf(msg, "User %d leaves chat", -cl_pid);
                if (arrlen(clients) == 0) {
                    printf("No clients left. Server shutting down.\n");
                    msgctl(*qid, IPC_RMID, 0);
                    exit(0);
                }
            }
            send_msg(*qid, clients, msg);
        }
    }
}
