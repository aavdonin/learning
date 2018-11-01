#include <ncurses.h>
#include "defines.h"

void draw_screen(char *buf, long startpos) {
    clear();
    int i;
    int eof = 0;
    char screen_text[LINES][COLS];
    for (i=0; i<LINES; i++) {
        int j = 0;
        while (j < COLS) {
            screen_text[i][j] = buf[startpos + j];
            if (buf[startpos + j] == '\0') break;
            if ((buf[startpos + j] == '\n') && (j + 1) < COLS ) {
                screen_text[i][j+1] = '\0';
                j++;
                break;
            }
            else j++;
        }
        startpos += j;
        printw("%s",screen_text[i]);
    }
    refresh();
}

void edit_mode(char *buf, long *startpos) {
    int key = KEY_UP;
    draw_screen(buf, *startpos);
    while ((key = getch()) != 27) { //While pressed key is not ESC

    }
}
