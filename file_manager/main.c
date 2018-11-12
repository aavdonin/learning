#include <ncurses.h>
#include "fs.h"
#include "ui.h"

int main(void) {
    initscr();
    start_color();
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

    struct file_rec *records = NULL;
    int rec_num;
    int selected = 1, startpos = 0;
    rec_num = get_dir_info(".", &records);
    print_list(win1, records, startpos, rec_num);
    selection(win1, selected, 1);

    int key;
    while ((key = getch()) != KEY_F(10)) {  //main cycle
        switch (key) {
            case KEY_UP:
            if (selected == 1) {
                if (startpos > 0) {
                    startpos--;
                    print_list(win1, records, startpos, rec_num); //redraw list
                    selection(win1, selected, 1);
                }
            }
            else {
                selection(win1, selected, 0);
                selection(win1, --selected, 1);
            }
            break;
            case KEY_DOWN:
            if (startpos + selected >= rec_num) break;
            int x,y;
            getmaxyx(win1, y, x);
            if (selected >= y - 1) {
                startpos++;
                print_list(win1, records, startpos, rec_num); //redraw list
                selection(win1, selected, 1);
            }
            else {
                selection(win1, selected, 0);
                selection(win1, ++selected, 1);
            }
            break;
        }
    }
        
    endwin();
    return 0;
}
