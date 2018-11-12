#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include "fs.h"
#include "ui.h"
#include "defines.h"

int main(void) {
    initscr();
    start_color();
    refresh();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    struct panel side[2];   //structure array for side panels

    WINDOW *borderwin1, *borderwin2;
    borderwin1 = newwin(LINES,COLS/2,0,0);
    borderwin2 = newwin(LINES,COLS/2,0,COLS/2);
    box(borderwin1,0,0);
    box(borderwin2,0,0);
    wrefresh(borderwin1);
    wrefresh(borderwin2);
    side[0].win = derwin(borderwin1,LINES-2,COLS/2-2,1,1);
    side[1].win = derwin(borderwin2,LINES-2,COLS/2-2,1,1);
    char active = 0;

    int i;
    for (i=0;i<2;i++) {     //side panels structure init
        side[i].records = NULL;
        side[i].selected = 1;
        side[i].startpos = 0;
        side[i].rec_num = get_dir_info(".", &(side[i].records));
        print_list(side[i].win, side[i].records, \
            side[i].startpos, side[i].rec_num);
        getcwd(side[i].path,MAXPATH);
    }
    selection(side[active].win, side[active].selected, 1);

    int key;
    struct panel *p;
    while ((key = getch()) != KEY_F(10)) {  //main cycle
        p = side + active;
        switch (key) {
            case KEY_UP:
            if (p->selected == 1) {
                if (p->startpos > 0) {
                    p->startpos--;
                    print_list(p->win, p->records, p->startpos, p->rec_num);
                    selection(p->win, p->selected, 1);
                }
            }
            else {
                selection(p->win, p->selected, 0);
                selection(p->win, --(p->selected), 1);
            }
            break;
            case KEY_DOWN:
            if (p->startpos + p->selected >= p->rec_num)
                break;
            int x,y;
            getmaxyx(p->win, y, x);
            if (p->selected >= y - 1) {
                (p->startpos)++;
                print_list(p->win, p->records, p->startpos, p->rec_num);
                selection(p->win, p->selected, 1);
            }
            else {
                selection(p->win, p->selected, 0);
                selection(p->win, ++(p->selected), 1);
            }
            break;
            case '\n':  //KEY_ENTER
            if (p->records[p->startpos + p->selected - 1].type == '/') {
                strcat(p->path,"/");
                strcat(p->path, \
                p->records[p->startpos + p->selected - 1].filename);
                chdir(p->path);
                p->rec_num = get_dir_info(".", &(p->records));
                p->startpos = 0;
                print_list(p->win, p->records, p->startpos, p->rec_num);
                selection(p->win, p->selected, 0);
                selection(p->win, p->selected = 1, 1);
            }
            break;
            case '\t':   //KEY_TAB
            active ^= 1; //invert value (0/1)
            selection(p->win, p->selected, 0); //deselect here
            p = side + active;
            selection(p->win, p->selected, 1); //and select there
            break;
        }
    }
        
    endwin();
    return 0;
}
