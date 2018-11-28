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
            }
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
            //todo: register and unregister clients in separate thread
            //make blocking queue reads
        }
    }
}
