#include <ncurses.h>
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

    struct panel panels[2];   //structure array for side panels

    WINDOW *borderwin1, *borderwin2;
    borderwin1 = newwin(LINES,COLS/2,0,0);
    borderwin2 = newwin(LINES,COLS/2,0,COLS/2);
    box(borderwin1,0,0);
    box(borderwin2,0,0);
    wrefresh(borderwin1);
    wrefresh(borderwin2);
    panels[0].win = derwin(borderwin1,LINES-2,COLS/2-2,1,1);
    panels[1].win = derwin(borderwin2,LINES-2,COLS/2-2,1,1);
    char active = 0;

    int i;
    for (i=0;i<2;i++) {     //side panels structure init
        panels[i].records = NULL;
        panels[i].selected = 1;
        panels[i].startpos = 0;
        panels[i].rec_num = get_dir_info(".", &(panels[i].records));
        print_list(panels + i);
        getcwd(panels[i].path,MAXPATH);
    }
    selection(panels[active].win, panels[active].selected, 1);

    int key;
    struct panel *p;
    while ((key = getch()) != KEY_F(10)) {  //main cycle
        p = panels + active;
        switch (key) {
            case KEY_UP:
                move_up(p);
                break;
            case KEY_DOWN:
                move_down(p);
                break;
            case '\n':  //KEY_ENTER
                enter_dir(p);
                break;
            case '\t':   //KEY_TAB
                active ^= 1; //invert value (0/1)
                selection(p->win, p->selected, 0); //deselect here
                p = panels + active;
                selection(p->win, p->selected, 1); //and select there
                break;
        }
    }
        
    endwin();
    return 0;
}
