#include <ncurses.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../defines.h"
#include "ui.h"

static void send_reg_msg(int qid, int pid);

WINDOW *chat, *input;

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr,"Too much arguments given!\n");
        fprintf(stderr,"Usage: %s [nickname]\n", argv[0]);
        return 1;
    }
    //open queue
    int msqid;
    message msg_out;
    if ((msqid = msgget(MSGQUEUEKEY, 0777)) < 0) {
        perror("msgget");
        return 1;
    }
    pid_t pid = getpid();       //get own pid
    send_reg_msg(msqid, pid);   //send pid to server
    init_screen();              //start graphics
    //make thread to recieve messages from queue and print to chatwindow
    pthread_t t_listener;
    int status;
    status = pthread_create(&t_listener, NULL, listenmsg, (void *)&msqid);
    if (status != 0) {
        endwin();
        perror("Unable to create 'listener' thread!\n");
        return 1;
    }

    //get message and send to server
    int key, pos, minpos;
    char msg[MSGSIZE];
    if (argc == 2) {
        strcpy(msg, argv[1]);
        strcat(msg, ": ");
        pos = minpos = strlen(argv[1]) + 2;
    }
    else {
        sprintf(msg, "%d: ", pid);
        pos = minpos = strlen(msg);
    }
    while ((key = getch()) != KEY_F(10)) {  //main cycle
        switch (key) {
        case '\n':  //KEY_ENTER
            memset(msg_out.mtext,0,MSGSIZE);
            msg_out.mtype = 2;
            strcpy(msg_out.mtext, msg);
            if (pos > minpos && msgsnd(msqid, &msg_out, pos, IPC_NOWAIT) < 0) {
                endwin();
                perror("msgsnd(out)");
                return 1;
            }
            pos = minpos;
            clear_input();
            break;
        case KEY_BACKSPACE: //backspace
            del_chr(msg, &pos, minpos);
            break;
        default:
            add_chr(key, msg, &pos);
            break;
        }
    }
    endwin();
    send_reg_msg(msqid, -pid);  //send unreg message
}

static void send_reg_msg(int qid, int pid) {
    message msg_reg;
    memset(msg_reg.mtext,0,MSGSIZE);
    msg_reg.mtype = 1;
    sprintf(msg_reg.mtext, "%d", pid);
    if (msgsnd(qid, &msg_reg, strlen(msg_reg.mtext), IPC_NOWAIT) < 0) {
        perror("msgsnd(reg)");
        exit(1);
    }
}
