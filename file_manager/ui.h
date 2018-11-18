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
void print_list(struct panel *p);
void init_panel(struct panel *p);
void selection(WINDOW *winptr, int line, char enabled);
void move_up(struct panel *p);
void move_down(struct panel *p);
void page_up(struct panel *p);
void page_down(struct panel *p);
void move_home(struct panel *p);
void move_end(struct panel *p);
void enter_dir(struct panel *p);
void init_screen(struct panel **p, char active);
void launch_editor(struct panel **p, char active, char editor_path[]);
void launch_binary(struct panel **p, char active);
void copy_file(struct panel **p, char active);
#endif
