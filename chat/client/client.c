#include <ncurses.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../defines.h"
#include "ui.h"

WINDOW *chat, *input;

int main(void) {
    init_screen();
    //open server.pipe
    int outpipe;
    if ((outpipe = open("server.pipe", O_WRONLY)) <= 0) {
        endwin();
        perror("Can't open server.pipe");
        return 1;
    }
    //get own pid
    pid_t pid = getpid();
    //create&open client_<pid>.pipe
    int inpipe;
    char pipefilename[MAXFILENAMELEN];
    sprintf(pipefilename, "client_%d.pipe", pid);
    if (mkfifo(pipefilename, 0777)) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }
    }
    if ((inpipe = open(pipefilename, O_RDONLY|O_NDELAY)) <= 0) {
        endwin();
        perror("Can't open client_%.pipe");
        return 1;
    }
    //make thread to listen from client.pipe and print to chatwindow
    pthread_t t_listener;
    int status;
    status = pthread_create(&t_listener, NULL, listenmsg, (void *)&inpipe);
    if (status != 0) {
        endwin();
        perror("Unable to create 'listener' thread!\n");
        return 1;
    }

    //put "!connect <pid>" into server.pipe
    char msg[MSGSIZE];
    sprintf(msg, "!connect %d", pid);
    if (write(outpipe, msg, strlen(msg)) <=0) {
        endwin();
        perror("Error writing to server.pipe");
        return 1;
    }
    //get message and put into server.pipe
    getch();
    endwin();
    remove(pipefilename);
}
