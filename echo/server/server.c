#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "serv_tcp.h"
#include "serv_udp.h"
#include "../defines.h"

int main (int argc, char *argv[]) {
    pthread_t t_tcp, t_udp;
    int status;
    //start thread for TCP socket
    status = pthread_create(&t_tcp, NULL, tcp_sock, NULL);
    if (status != 0) {
        perror("Unable to create 't_tcp' thread!\n");
        return 1;
    }
    if (pthread_detach(t_tcp) != 0) {
        perror("Unable to detach 't_tcp' thread!\n");
        return 1;
    }
    //start thread for UDP socket
    status = pthread_create(&t_udp, NULL, udp_sock, NULL);
    if (status != 0) {
        perror("Unable to create 't_udp' thread!\n");
        return 1;
    }
    if (pthread_detach(t_udp) != 0) {
        perror("Unable to detach 't_tcp' thread!\n");
        return 1;
    }

    //wait for "exit" command
    char cmd[CMDSIZE];
    //printf("server> ");
    while(strcmp(fgets(cmd, CMDSIZE, stdin),"quit\n") != 0) {
        printf("Wrong command. Type 'quit' to shutdown the server.\n");
        printf("server> ");
    }
    exit(0);
}
