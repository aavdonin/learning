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
    wprintw(winptr, "%c%-25s %.0ld", rec.type, rec.filename, rec.size);
}
