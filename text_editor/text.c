#include <ncurses.h>
#include <string.h>
#include "defines.h"

void draw_screen(char *buf, long startpos) {
    clear();
    int i;
    char screen_text[LINES][COLS];
    for (i=0; i<LINES; i++) {
        int j = 0;
        while ((j < COLS) && (buf[startpos + j] != '\0')) {
            screen_text[i][j] = buf[startpos + j];
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
}

void edit_mode(char *buf) {

}
