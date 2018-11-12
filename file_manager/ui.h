#ifndef _UI_H_
#define _UI_H_

#include <ncurses.h>
#include "defines.h"
#include "fs.h"

struct panel {
    WINDOW *win;
    struct file_rec *records;
    int rec_num;
    int selected;
    int startpos;
    char path[MAXPATH];
};
void exit_failure(char *message);
void print_rec(WINDOW *winptr, struct file_rec rec);
void print_list(WINDOW *winptr, struct file_rec *records, int startpos, \
int rec_num);
void selection(WINDOW *winptr, int line, char enabled);
#endif
