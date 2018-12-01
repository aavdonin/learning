#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

extern WINDOW *chat, *input;

struct sembuf get_sembuf(unsigned short sem_num, short sem_op, short sem_flg) {
    struct sembuf buf;
    buf.sem_num = sem_num;
    buf.sem_op = sem_op;
    buf.sem_flg = sem_flg;
    return buf;
}

static void put_line(char *text) {
    int x, y, curs_x, curs_y;
    getmaxyx(chat,y,x);
    getyx(input, curs_y, curs_x);
    mvwprintw(chat, y-1, x-1, "\n%s", text);  //print to chat win
    wmove(input, curs_y, curs_x);
    wrefresh(chat);
    wrefresh(input);
}
void *listenmsg(void *arg) {
    mem_t *shmemory = (mem_t *) arg;
    key_t ipckey = ftok("shmchat.c", 2);
    int semid = semget(ipckey, 1, 0777);

    int x, y, curs_x, curs_y;
    getmaxyx(chat,y,x);
    getyx(input, curs_y, curs_x);
    while (1) {
        struct sembuf op;
        op = get_sembuf(MA, -1, 0);  //decrease MA by 1
        semop(semid, &op, 1);
        mvwprintw(chat, y-1, x-1, "\n%s", shmemory->text);  //print to chat win
        wmove(input, curs_y, curs_x);
        wrefresh(chat);
        wrefresh(input);

        op = get_sembuf(MA, 0, 0); // wait for MA=0
        semop(semid, &op, 1);
    }
}

void init_screen(void) {
    initscr();
    clear();
    refresh();
    noecho();
    keypad(stdscr, TRUE);

    WINDOW *borderwin1, *borderwin2;    //draw borders
    borderwin1 = newwin(LINES-CHAT_INPUT_HEIGHT,COLS,0,0);
    borderwin2 = newwin(CHAT_INPUT_HEIGHT,COLS,LINES-CHAT_INPUT_HEIGHT,0);
    box(borderwin1,0,0);
    box(borderwin2,0,0);
    wrefresh(borderwin1);
    wrefresh(borderwin2);
    int x, y;
    getmaxyx(borderwin1,y,x);
    chat = derwin(borderwin1,y-2,x-2,1,1);
    scrollok(chat,TRUE);
    getmaxyx(borderwin2,y,x);
    input = derwin(borderwin2,y-2,x-2,1,1);
    wrefresh(chat);
    wrefresh(input);
    
    wmove(input,0,0);
}

void clear_input(void) {
    werase(input);
    wmove(input,0,0);
    wrefresh(input);
}

void add_chr(int key, char *msg, int *pos) {
    if ((key >= 0x20 && key <= 0x7E) || key == '\n') { //printable chars
        if (*pos < MSGSIZE-1) {
            msg[*pos] = key;
            wprintw(input, "%c", key);
            (*pos)++;
            msg[*pos] = '\0';
        }
    }
    wrefresh(input);
}

void del_chr(char *msg, int *pos, int minpos) {
    int x, y, max_x, max_y;
    if (*pos > minpos) {
        getyx(input, y, x);
        getmaxyx(input, max_y, max_x);
        (*pos)--;
        wmove(input, (*pos - minpos)/max_x, (*pos - minpos)%max_x);
        wclrtobot(input);
    }
    wrefresh(input);
}
