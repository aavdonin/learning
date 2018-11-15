#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "fs.h"
#include "ui.h"

void exit_failure(char *message) {
    //print *message and abort program
    clear();
    printw("%s", message);
    getch();
    endwin();
    exit(1);
}

void print_rec(WINDOW *winptr, struct file_rec rec) {
    //print single record
    int x, y;
    getmaxyx(winptr, y, x);
    wprintw(winptr, "%c%-*.*s%*.0ld\n", rec.type, \
    x*2/3-1, x*2/3-1, rec.filename, \
    x/3-1, rec.size);
}

void print_list(struct panel *p) {
    //print header and whole list of records
    int x,y,i;
    getmaxyx(p->win, y, x);
    mvwprintw(p->win,0,0," %-*s%-*s\n", x*2/3-1, "Name", x/3-1, "Size");
    for (i=0; (p->startpos + i < p->rec_num) && (i < y-1); i++) {
        print_rec(p->win, p->records[p->startpos + i]);
    }
    wclrtobot(p->win);
    wrefresh(p->win);
}

void init_panel(struct panel *p) {
    p->records = NULL;
    p->selected = 1;
    p->startpos = 0;
    p->rec_num = get_dir_info(".", &(p->records));
    print_list(p);
    getcwd(p->path,MAXPATH);
}

void selection(WINDOW *winptr, int line, char enabled) {
    //select or deselect specified line
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    mvwchgat(winptr, line, 0, -1, COLOR_PAIR(1), enabled, NULL);
    move(0,0);
    wrefresh(winptr);
}

void move_up(struct panel *p) {
    if (p->selected == 1) {
        if (p->startpos > 0) {
            p->startpos--;
            print_list(p);
            selection(p->win, p->selected, 1);
        }
    }
    else {
        selection(p->win, p->selected, 0);
        selection(p->win, --(p->selected), 1);
    }
}

void move_down(struct panel *p) {
    if (p->startpos + p->selected >= p->rec_num)
        return;
    int x,y;
    getmaxyx(p->win, y, x);
    if (p->selected >= y - 1) {
        (p->startpos)++;
        print_list(p);
        selection(p->win, p->selected, 1);
    }
    else {
        selection(p->win, p->selected, 0);
        selection(p->win, ++(p->selected), 1);
    }
}

void page_up(struct panel *p) {
    int x,y;
    getmaxyx(p->win, y, x); y--;
    if (p->startpos == 0) {
        selection(p->win, p->selected, 0);
        p->selected = 1;
    }
    else {
        p->startpos -= y;
        if (p->startpos < 0) {
            p->startpos = 0;
            p->selected = 1;
        }
        print_list(p);
    }
    selection(p->win, p->selected, 1);
}

void page_down(struct panel *p) {
    int x,y;
    getmaxyx(p->win, y, x); y--;
    if ((p->rec_num - p->startpos) <= y) {
        selection(p->win, p->selected, 0);
        p->selected = p->rec_num - p->startpos;
    }
    else {
        p->startpos += y;
        print_list(p);
        if (p->selected > (p->rec_num - p->startpos))
            p->selected = p->rec_num - p->startpos;
    }
    selection(p->win, p->selected, 1);
}

void move_home(struct panel *p) {
    p->startpos = 0;
    p->selected = 1;
    print_list(p);
    selection(p->win, p->selected, 1);
}

void move_end(struct panel *p) {
    int x,y;
    getmaxyx(p->win, y, x); y--;
    if ((p->rec_num > y) && (p->startpos + y <= p->rec_num)) {
        p->startpos = p->rec_num - y;
        p->selected = y;
        print_list(p);
        selection(p->win, p->selected, 1);
    }
    else
        page_down(p);
}

void enter_dir(struct panel *p) {
    if (p->records[p->startpos + p->selected - 1].type == '/') {
        strcat(p->path,"/");
        int selected = p->startpos + p->selected - 1;
        strcat(p->path, p->records[selected].filename);
        chdir(p->path);
        p->rec_num = get_dir_info(".", &(p->records));
        p->startpos = 0;
        print_list(p);
        selection(p->win, p->selected, 0);
        selection(p->win, p->selected = 1, 1);
    }
    else {  //open file in text editor
        pid_t pid;
        switch (pid = fork()) {
        case -1:
            exit_failure("Error occured while forking process\n");
            break;
        case 0:
            ;int selected = p->startpos + p->selected - 1;
            execl("../text_editor/editor", "editor", \
            p->records[selected].filename, NULL);
            break;
        default:
            wait(NULL);
            break;
        }
    }
}
