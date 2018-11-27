#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include "../defines.h"

extern WINDOW *chat, *input;

void *listenmsg(void *arg) {
    int *inpipe = (int*) arg;
    int x, y, curs_x, curs_y;
    getmaxyx(chat,y,x);
    getyx(input, curs_y, curs_x);
    int len;
    char msg[MSGSIZE];
    while (1) {
        if ((len = read(*inpipe, msg, MSGSIZE)) > 0) {
            msg[len] = '\0';
            mvwprintw(chat, y-1, x-1, "\n%s", msg);
            wmove(input, curs_y, curs_x);
            wrefresh(chat);
            wrefresh(input);
        }
        else {
            struct timespec delay;
            delay.tv_sec = 0;
            delay.tv_nsec = 100000000L; //0.1sec
            nanosleep(&delay,NULL);
        }
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
        if (*pos < MSGSIZE) {
            msg[*pos] = key;
            wprintw(input, "%c", key);
            (*pos)++;
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