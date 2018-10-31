#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

FILE *file;
char filename[FILENAME_MAX_LEN];

int main(void) {
    int menu_item = -1;
    char **bufptr;
    char *buf;
    bufptr = &buf;
    long bufsize, bufused;
    initscr();
    noecho();
    if (!has_colors()) {
        endwin();
        printf("There's no colors here :(\n");
        return 1;
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    while (menu_item != '5') {
        if (menu_item != '3') 
            print_menu();
        menu_item = getch();
        switch (menu_item) {
            case '1':        //open file
                if (file) break;
                attron(COLOR_PAIR(2));
                mvprintw(1, 8, "Filename: ");
                refresh();
                echo();
                attroff(COLOR_PAIR(2));
                getstr(filename);
                noecho();
                file = fopen(filename, "r");
                if (!file) {
                    attron(COLOR_PAIR(1));
                    mvprintw(1, 8, "Can't open file!");
                    getch();
                    attroff(COLOR_PAIR(1));
                    break;
                }
                bufsize = load_text(file, bufptr);
                break;
            case '2':        //edit
                if (file) {
                    clear();
                    mvprintw(0,0,"%s",buf);
                    refresh();
                    getch();
                }
                break;
            case '3':        //save
                if (file) {
                    attron(COLOR_PAIR(2));
                    mvprintw(3, 8, "OK!");
                    attroff(COLOR_PAIR(2));
                    move(6, 0);
                }
                break;
            case '4':        //close
                if (file) {
                    fclose(file);
                    file = NULL;
                    *filename = '\0';
                    free(buf);
                }
                break;
        }
    }
    endwin();
    return 0;
}

void print_menu(void) {
    clear();
    printw("Text editor\n");
    printw("1) Open");
    if (file) {
        printw("ed: ");
        attron(COLOR_PAIR(1));
        printw("%s\n", filename);
        attroff(COLOR_PAIR(1));
    }
    else printw("\n");
    printw("2)%s", file ? " Edit\n" : "\n");
    printw("3)%s", file ? " Save\n" : "\n");
    printw("4)%s", file ? " Close\n" : "\n");
    printw("5) Exit\n");
    refresh();
}
