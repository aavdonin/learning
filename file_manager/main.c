#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

static int filter_func(const struct dirent *entry) {
    return strcmp(entry->d_name,".");
}

int main(void) {
    int num, i;
    struct dirent **entry;
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    printw("Soon I'll become a file manager!\n");
    num = scandir(".", &entry, filter_func, alphasort);
    if (num<0) {
        printw("Directory read error");
        getch();
        endwin();
        return 1;
    }
    for (i = 0; i < num; i++) {
        printw("%20s inode:%i\n", entry[i]->d_name, entry[i]->d_ino);
        free(entry[i]);
    }
    free(entry);

    refresh();
    getch();
    endwin();
    return 0;
}