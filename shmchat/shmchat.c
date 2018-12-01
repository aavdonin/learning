#include <ncurses.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "defines.h"
#include "ui.h"

WINDOW *chat, *input;

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr,"Too much arguments given!\n");
        fprintf(stderr,"Usage: %s [nickname]\n", argv[0]);
        return 1;
    }
    //create shm if needed and attach to it
    char shm_exist = 0;
    key_t ipc_key = ftok("shmchat.c", 1);
    int shmid = shmget(ipc_key, sizeof(mem_t), 0777 | IPC_CREAT | IPC_EXCL);
    if (shmid < 0) {
        if (errno == EEXIST) {
            shm_exist = 1;
            shmid = shmget(ipc_key, sizeof(mem_t), 0777 | IPC_CREAT);
        }
        else {
            perror("shmget");
            return 1;
        }
    }
    mem_t *shmemory;
    if ((shmemory = shmat(shmid, (void *)0, 0)) == (mem_t *) -1) {
        perror("shmat");
        return 1;
    }
    if (!shm_exist) {
        shmemory->client_num = 0;
        memset(shmemory->text, 0, MSGSIZE);
    }

    //create and init semaphores
    ipc_key = ftok("shmchat.c", 2);
    int semid = semget(ipc_key, 1, 0777 | IPC_CREAT);
    if (!shm_exist) {
        union semun sem_controller;
        sem_controller.val = 0;
        //MA = 0 - memory access semaphore, initially set to 0
        semctl(semid, MA, SETVAL, sem_controller);
    }
    //add yourself to client_num
    struct sembuf op;
    op = get_sembuf(MA, 0, 0); // wait for MA=0
    semop(semid, &op, 1);
    shmemory->client_num++;

    init_screen();              //start graphics
    //make thread to recieve messages from queue and print to chatwindow
    pthread_t t_listener;
    int status;
    status = pthread_create(&t_listener, NULL, listenmsg, (void *)shmemory);
    if (status != 0) {
        endwin();
        perror("Unable to create 'listener' thread!\n");
        return 1;
    }

    //get message and send to others
    int key, pos, minpos;
    char msg[MSGSIZE];
    if (argc == 2) {
        strcpy(msg, argv[1]);
        strcat(msg, ": ");
        pos = minpos = strlen(argv[1]) + 2;
    }
    else {
        sprintf(msg, "%d: ", getpid());
        pos = minpos = strlen(msg);
    }
    while ((key = getch()) != KEY_F(10)) {  //main cycle
        switch (key) {
        case '\n':  //KEY_ENTER
            if (pos <= minpos) break;
            struct sembuf op;
            op = get_sembuf(MA, 0, 0); // wait for MA=0
            semop(semid, &op, 1);

            memset(shmemory->text, 0, MSGSIZE);
            strcpy(shmemory->text, msg);

            op = get_sembuf(MA, shmemory->client_num, 0); // unlock whole sem MA
            semop(semid, &op, 1);

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
    //del yourself from client_num
    op = get_sembuf(MA, 0, 0); // wait for MA=0
    semop(semid, &op, 1);
    shmemory->client_num--;
    //destroy shared memory and semaphores if you're the last
    if (shmemory->client_num == 0) {
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
    }
}

