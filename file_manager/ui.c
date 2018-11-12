#include <ncurses.h>
#include <stdlib.h>
#include "fs.h"

void exit_failure(char *message) {
    clear();
    printw("%s", message);
    getch();
    endwin();
    exit(1);
}

void print_rec(WINDOW *winptr, struct file_rec rec) {
    int x, y;
    getmaxyx(winptr, y, x);
    wprintw(winptr, "%c%-*s%*.0ld\n", rec.type, x*2/3-1, rec.filename, \
    x/3-1, rec.size);
}

void print_header(WINDOW *winptr) {
    int x,y;
    getmaxyx(winptr, y, x);
    mvwprintw(winptr,0,0," %-*s%-*s\n", x*2/3-1, "Name", x/3-1, "Size");
    wrefresh(winptr);
}

void selection(WINDOW *winptr, int line, char enabled) {
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    mvwchgat(winptr, line, 0, -1, COLOR_PAIR(1), enabled, NULL);
    move(0,0);
    wrefresh(winptr);
}