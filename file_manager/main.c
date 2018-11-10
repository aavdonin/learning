#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include "fs.h"
#include "ui.h"

int main(void) {
    int num, i;
    struct dirent **entry;
    struct file_rec record;
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    printw("Soon I'll become a file manager!\n");
    num = scandir(".", &entry, filter_func, alphasort);
    if (num<0) {
        exit_failure("Directory read error");
    }
    for (i = 0; i < num; i++) {
        record = get_rec(entry[i]->d_name);
        print_rec(record);
        printw("\n");
        free(entry[i]);
    }
    free(entry);

    refresh();
    getch();
    endwin();
    return 0;
}
