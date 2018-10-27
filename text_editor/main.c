#include <ncurses.h>

int main(void) {
    initscr();
    if (!has_colors()) {
        endwin();
        printf("There's no colors here :(\n");
        return 1;
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    printw("Hello world!\n");
    refresh();
    attroff(COLOR_PAIR(1));
    getch();
    endwin();

    return 0;
};

