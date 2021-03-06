#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <pthread.h>
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
    strcat(p->path,"/");
    int selected = p->startpos + p->selected - 1;
    strcat(p->path, p->records[selected].filename);
    chdir(p->path);
    getcwd(p->path,MAXPATH);
    p->rec_num = get_dir_info(".", &(p->records));
    p->startpos = 0;
    print_list(p);
    selection(p->win, p->selected, 0);
    selection(p->win, p->selected = 1, 1);
}

void launch_editor(struct panel **p, char active, char editor_path[MAXPATH]) {
    //open file in text editor
    pid_t pid;
    int selected = (*p)->startpos + (*p)->selected - 1;
    char *filename = (*p)->records[selected].filename;
    endwin();
    switch (pid = fork()) {
    case -1:
        exit_failure("Cannot make new process\n");
        break;
    case 0:
        execl(editor_path, "editor", filename, NULL);
        exit_failure("Cannot load editor binary\n");
    default:
        wait(NULL);
        init_screen(p, active);
        break;
    }
}

void launch_binary(struct panel **p, char active) {
    //launch specified binary
    pid_t pid;
    int selected = (*p)->startpos + (*p)->selected - 1;
    char *filename = (*p)->records[selected].filename;
    endwin();
    switch (pid = fork()) {
    case -1:
        exit_failure("Cannot make new process\n");
        break;
    case 0:
        execl(filename, filename, NULL);
        exit_failure("Cannot execute binary\n");
    default:
        wait(NULL);
        init_screen(p, active);
        break;
    }
}

void init_screen(struct panel **p, char active) {
    initscr();
    clear();
    start_color();
    refresh();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    WINDOW *borderwin1, *borderwin2;    //draw borders
    borderwin1 = newwin(LINES,COLS/2,0,0);
    borderwin2 = newwin(LINES,COLS/2,0,COLS/2);
    box(borderwin1,0,0);
    box(borderwin2,0,0);
    wrefresh(borderwin1);
    wrefresh(borderwin2);
    //structure array for side panels:
    struct panel *panels;
    if (!*p) {
        panels = malloc(sizeof(struct panel) * 2);
        *p = panels + active;
        if (!panels)
            exit_failure("Memory allocation failure");
        init_panel(panels);
        init_panel(panels + 1);
    }
    else {
        panels = (*p) - active;
    }
    panels[0].win = derwin(borderwin1,LINES-2,COLS/2-2,1,1);
    panels[1].win = derwin(borderwin2,LINES-2,COLS/2-2,1,1);
    print_list(&panels[0]);
    print_list(&panels[1]);
    selection((*p)->win, (*p)->selected, 1);
}

void copy_file(struct panel **p, char active) {
    int status, *cp_result;
    pthread_t cp_thrd, progress_thrd;
    pthread_attr_t attr;
    cp_args arg;
    struct panel *p_from, *p_to;
    p_from = *p;
    p_to = *p + (active ? -1 : 1);
    strcpy(arg.from, p_from->path);
    strcat(arg.from, "/");
    int selected = p_from->startpos + p_from->selected - 1;
    strcat(arg.from, p_from->records[selected].filename);
    strcpy(arg.to, p_to->path);
    strcat(arg.to, "/");
    strcat(arg.to, p_from->records[selected].filename);

    if (p_from->records[selected].type == '/') //do not copy directories
        return;

    if (pthread_attr_init(&attr) != 0 ) {
        exit_failure("Unable to initialize thread attributes\n");
    }
    status = pthread_create(&cp_thrd, &attr, copy, &arg);
    if (status != 0) {
        exit_failure("Unable to create 'copy' thread!\n");
    }
    status = pthread_create(&progress_thrd, &attr, copy_progress, &arg);
    if (status != 0) {
        exit_failure("Unable to create 'copy_progress' thread!\n");
    }
    status = pthread_join(cp_thrd, (void **) &cp_result);
    if (status != 0) {
        exit_failure("Join 'copy' thread error\n");
    }
    if (*cp_result != 0) {
        exit_failure("File copy failed\n");
    }
    free(cp_result);
    pthread_join(progress_thrd, (void **) &cp_result);
    free(cp_result);
    endwin();
    init_screen(p, active);
    chdir(p_to->path);
    p_to->rec_num = get_dir_info(p_to->path, &(p_to->records));
    chdir(p_from->path);
    print_list(p_to);
}

void *copy_progress(void *args) {
    //shows percentage of copying progress
    cp_args *arg = (cp_args *)args;
    WINDOW *progress_win = newwin(7,19,(LINES-7)/2,(COLS-19)/2);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_BLACK);
    wbkgd(progress_win,COLOR_PAIR(1));
    box(progress_win,0,0);
    mvwprintw(progress_win,2,2,"Copying file...");
    wrefresh(progress_win);
    char ratio = 0;
    while (1) {
        struct file_rec src, dest;
        src  = get_rec(arg->from);
        dest = get_rec(arg->to);
        ratio = (char) 100 * ((float) dest.size / src.size);
        int black = ratio / 10;
        int white = 10 - black;
        if (black > 0) {
            wattron(progress_win,COLOR_PAIR(3));
            mvwprintw(progress_win,4,2,"%*s",black," ");
        }
        else
            wmove(progress_win,4,2);
        wattron(progress_win,COLOR_PAIR(2));
        wprintw(progress_win,"%*s",white," ");
        wattron(progress_win,COLOR_PAIR(1));
        mvwprintw(progress_win,4,13,"%3d%%",ratio);
        wrefresh(progress_win);
        if (src.size <= dest.size) break;
    }

    int *result = malloc(sizeof(int));
    *result = 0;
    return (void *) result;
}
