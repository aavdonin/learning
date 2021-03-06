#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "fs.h"
#include "ui.h"
#include "defines.h"

int main(void) {
    char active = 0;                    //0 - left, 1 - right
    struct panel *p = NULL;  //pointer to active panel
    init_screen(&p, active);
    struct panel *p1 = p - active; //pointer to left panel
    char editor_path[MAXPATH];
    getcwd(editor_path,MAXPATH);
    strcat(editor_path,"/../text_editor/editor");

    int key;
    while ((key = getch()) != KEY_F(10)) {  //main cycle
        switch (key) {
            case KEY_UP:
                move_up(p);
                break;
            case KEY_DOWN:
                move_down(p);
                break;
            case KEY_LEFT:
                page_up(p);
                break;
            case KEY_RIGHT:
                page_down(p);
                break;
            case KEY_PPAGE: //pg_up
                page_up(p);
                break;
            case KEY_NPAGE: //pg_down
                page_down(p);
                break;
            case KEY_HOME:
                move_home(p);
                break;
            case KEY_END:
                move_end(p);
                break;
            case '\n':  //KEY_ENTER
                if (p->records[p->startpos + p->selected - 1].type == '/')
                    enter_dir(p);
                else if (p->records[p->startpos + p->selected - 1].type == '*')
                    launch_binary(&p, active);
                else
                    launch_editor(&p, active, editor_path);
                break;
            case '\t':   //KEY_TAB
                active ^= 1; //invert value (0/1)
                selection(p->win, p->selected, 0); //deselect here
                p = p1 + active;
                chdir(p->path);
                selection(p->win, p->selected, 1); //and select there
                break;
            case KEY_F(5):
                copy_file(&p, active);
                break;
        }
    }
        
    endwin();
    return 0;
}
