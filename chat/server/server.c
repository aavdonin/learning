#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "../defines.h"
#include "array_fnc.h"
#include "hostess.h"

int *clients = NULL;

int main(int argc, char *argv[]) {
    int msqid;
    message msg_in;
    if ((msqid = msgget(MSGQUEUEKEY, IPC_CREAT | 0777)) < 0) {
        perror("msgget");
        return 1;
    }
    //run hostess thread
    pthread_t t_hostess;
    int status;
    status = pthread_create(&t_hostess, NULL, hostess, (void *)&msqid);
    if (status != 0) {
        perror("Unable to create 'hostess' thread!\n");
        return 1;
    }
    while (1) { //main cycle
        memset(msg_in.mtext,0,MSGSIZE);
        if (msgrcv(msqid, &msg_in, MSGSIZE, 2, 0) < 0) {
            if (errno != ENOMSG && errno != EIDRM && errno != EINVAL)
                perror("msgrcv(in)");
        }
        else {
            //send received message to all registered clients
            send_msg(msqid, clients, msg_in.mtext);
        }
    }
}
