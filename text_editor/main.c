#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

FILE *file;
char filename[FILENAME_MAX_LEN];

int main(int argc, char *argv[]) {
    if (argc == 2) {        //handle given arguments first
        strcpy(filename, argv[1]);
        file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr,"Unable to open file <%s>\n", argv[1]);
            exit(1);
        }
    }
    else if (argc > 2) {
        fprintf(stderr,"Too much arguments given!\n");
        fprintf(stderr,"Usage: %s [file]\n", argv[0]);
        exit(1);
    }
    int menu_item = -1;
    char *buf;              //buffer to hold file contents
    long bufsize, bufused;  //buffer size and count of bytes used
    long startpos = 0;      //position to start file displaying from
    if (file) bufsize = load_text(file, &buf, &bufused); //load file if opened
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    if (argc == 2) {        //if opened with command line argument
                            //goto edit mode immediately
        edit_mode(&buf,&startpos, &bufused, &bufsize);
    }
    if (!has_colors()) {
        endwin();
        printf("There's no colors here :(\n");
        return 1;
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    while (menu_item != '5') {  //main menu routine
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
                bufsize = load_text(file, &buf, &bufused);
                break;
            case '2':        //edit
                if (file) {
                    edit_mode(&buf,&startpos, &bufused, &bufsize);
                }
                break;
            case '3':        //save
                if (file) {
                    file = freopen(filename, "w", file);
                    save_text(file, buf, bufused);
                    file = freopen(filename, "r", file);
                    if (file) {
                        attron(COLOR_PAIR(2));
                        mvprintw(3, 8, "OK!");
                        attroff(COLOR_PAIR(2));
                        move(6, 0);
                    }
                }
                break;
            case '4':        //close
                if (file) {
                    fclose(file);
                    file = NULL;
                    *filename = '\0';
                    free(buf);
                    startpos = 0;
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
