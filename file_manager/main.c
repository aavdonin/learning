#include <ncurses.h>
#include "defines.h"

int main(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    if (has_colors()) 
        start_color();
    getch();
    return 0;
}