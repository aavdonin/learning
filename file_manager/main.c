#include <ncurses.h>
#include "fs.h"
#include "ui.h"

int main(void) {
    initscr();
    refresh();
    noecho();
    keypad(stdscr, TRUE);

    WINDOW *borderwin1, *borderwin2;
    borderwin1 = newwin(LINES,COLS/2,0,0);
    borderwin2 = newwin(LINES,COLS/2,0,COLS/2);
    box(borderwin1,0,0);
    box(borderwin2,0,0);
    wrefresh(borderwin1);
    wrefresh(borderwin2);
    WINDOW *win1, *win2;
    win1 = derwin(borderwin1,LINES-2,COLS/2-2,1,1);
    win2 = derwin(borderwin2,LINES-2,COLS/2-2,1,1);
    mvwprintw(win2,0,0,"Soon I'll become a file manager!\n");
    wrefresh(win2);

    mvwprintw(win1,0,0,"Name       Size");
    wrefresh(win1);

    move(3,0);
    struct file_rec *records = NULL;
    int rec_num, i;
    rec_num = get_dir_info(".", &records);
    for (i=0;i<rec_num;i++) {
        print_rec(records[i]);
        printw("\n");
    }

    refresh();
    getch();
    endwin();
    return 0;
}
