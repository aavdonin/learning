#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"
#include "text.h"

void draw_screen(char *buf, char screen_text[LINES][COLS+1], long startpos) {   //function to fill screen_text array and print it on screen
    clear();                                                                    //from buffer starting with startpos'th character
    int i;
    for (i=0; i<LINES; i++) {
        int j = 0;
        while (j < COLS) {
            screen_text[i][j] = buf[startpos + j];
            if (buf[startpos + j] == '\0') break;
            if ((buf[startpos + j] == '\n') && ((COLS - j) > 1)) {
                screen_text[i][++j] = '\0';
                break;
            }
            j++;
            if (j == COLS) screen_text[i][j] = '\0';
        }
        startpos += j;
        printw("%s",screen_text[i]);
    }
    refresh();
}

void scroll_up(char *buf, char screen_text[LINES][COLS+1], long *startpos) {  //finds the beginnig of previous line
    if (*startpos > 0) {                                                      //changes screen_text, *startpos, resets cursor position
        int newlines = 2;
        while (--(*startpos)) {
            if (buf[*startpos] == '\n') newlines--;
            if (newlines == 0) {
                (*startpos)++;
                break;
            }
        }
        draw_screen(buf, screen_text, *startpos);
    }
}

void edit_mode(char **bufptr, long *startpos, long *bufused, long *bufsize) {   //edit mode routine
    char *buf;
    buf = *bufptr;
    int key = 0;    //pressed key code
    int x, y;       //coordinates for cursor positioning
    char screen_text[LINES][COLS+1];    //array of chars to display on screen
    long curs_pos = 0;  //position of cursor
    char insert_flag = 1;   //0 - replace, 1 - insert
    curs_set(insert_flag + 1);
    draw_screen(buf, screen_text, *startpos);
    move(0,0);
    curs_pos = 0;
    while ((key = getch()) != 27) { //Getting user input, while pressed key is not ESC
        switch (key) {
            case KEY_UP:
                getyx(stdscr,y,x);
                if (y == 0) {
                    if (*startpos <= 0) 
                        curs_pos = 0;
                    else {
                        scroll_up(buf, screen_text, startpos);
                        curs_pos = get_curs_pos_atxy(x, y, screen_text);
                    }
                }
                else
                    curs_pos = get_curs_pos_atxy(x, y-1, screen_text);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_DOWN:
                getyx(stdscr,y,x);
                if (y == LINES-1) {
                    (*startpos) += strlen(screen_text[0]);
                    draw_screen(buf, screen_text, *startpos);
                    curs_pos = get_curs_pos_atxy(x, y, screen_text);
                }
                else
                    curs_pos = get_curs_pos_atxy(x, y+1, screen_text);
                if (((*startpos) + curs_pos) >= ((*bufused) - 2)) curs_pos = (*bufused) - (*startpos) - 2;
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_LEFT:
                getyx(stdscr,y,x);
                if (y == 0 && x ==0 && *startpos > 0) {
                    scroll_up(buf, screen_text, startpos);
                    curs_pos = get_curs_pos_atxy(COLS, y, screen_text);
                    move_curs_to(curs_pos, screen_text);
                }
                else
                    move_curs_to(--curs_pos, screen_text);
                break;
            case KEY_RIGHT:
                if (((*startpos) + curs_pos) >= ((*bufused) - 2)) break;
                getyx(stdscr,y,x);
                    if (y == LINES-1 && x == strlen(screen_text[y])-1) {
                        (*startpos) += strlen(screen_text[0]);
                        draw_screen(buf, screen_text, *startpos);
                        curs_pos = get_curs_pos_atxy(0, y, screen_text);
                        move_curs_to(curs_pos, screen_text);
                    }
                else
                    move_curs_to(++curs_pos, screen_text);
                break;
            case KEY_HOME:
                getyx(stdscr,y,x);
                curs_pos = get_curs_pos_atxy(0, y, screen_text);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_END:
                getyx(stdscr,y,x);
                curs_pos = get_curs_pos_atxy(COLS, y, screen_text);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_DC:    //delete
                del_chr(buf, bufused, (*startpos) + curs_pos);
                draw_screen(buf, screen_text, *startpos);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_BACKSPACE: //backspace
                getyx(stdscr,y,x);
                if (y == 0 && x ==0 && *startpos > 0) {
                    scroll_up(buf, screen_text, startpos);
                    curs_pos = get_curs_pos_atxy(COLS, y, screen_text);
                }
                else
                    curs_pos--;
                del_chr(buf, bufused, (*startpos) + curs_pos);
                draw_screen(buf, screen_text, *startpos);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_PPAGE:     //page up
                getyx(stdscr,y,x);
                if (*startpos <= 0)
                    curs_pos = 0;
                else {
                    long seek = (*startpos);
                    while ((*startpos) > 0 && ((*startpos) + get_curs_pos_atxy(COLS+1, LINES, screen_text)) != seek)
                        scroll_up(buf, screen_text, startpos);
                    curs_pos = get_curs_pos_atxy(x, y, screen_text);
                }
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_NPAGE:     //page down
                getyx(stdscr,y,x);
                curs_pos = get_curs_pos_atxy(COLS+1, LINES, screen_text);
                if ((*startpos) + curs_pos < (*bufused) - 2) {
                    (*startpos) += curs_pos;
                    draw_screen(buf, screen_text, *startpos);
                    curs_pos = get_curs_pos_atxy(x, y, screen_text);
                }
                else
                    curs_pos = (*bufused) - (*startpos) - 2;
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_IC:    //Insert
                insert_flag ^= 1; //invert flag
                curs_set(insert_flag + 1);
            default:
                if ((key >= 0x20 && key <= 0x7E) || key == '\n') {      //printable chars
                    if (insert_flag) {      //insert mode
                        add_chr(bufptr, bufused, bufsize, (*startpos) + curs_pos, key);
                        draw_screen(buf, screen_text, *startpos);
                        move_curs_to(++curs_pos, screen_text);
                    }
                    else {                  //replace mode
                        buf[(*startpos) + curs_pos] = key;
                        getyx(stdscr,y,x);
                        draw_screen(buf, screen_text, *startpos);
                        if (((*startpos) + curs_pos) >= ((*bufused) - 2)) break;
                        if (y == LINES-1 && x == strlen(screen_text[y])-1) {
                            (*startpos) += strlen(screen_text[0]);
                            draw_screen(buf, screen_text, *startpos);
                            curs_pos = get_curs_pos_atxy(0, y, screen_text);
                            move_curs_to(curs_pos, screen_text);
                        }
                        else
                            move_curs_to(++curs_pos, screen_text);
                    }
                }
                break;
        }
    }
}

void del_chr(char *buf, long *bufused, long position) {     //deletes char from buffer at specified position
    if (position >= *bufused) {
        fprintf(stderr,"Erasing out of buffer bounds!\n");
        exit(1);
    }
    memmove(buf + position, buf + position + 1, *bufused - position);
    (*bufused)--;
}

void add_chr(char **bufptr, long *bufused, long *bufsize, long position, char chr) {    //adds char to buffer at specified position
    char *buf = *bufptr;
    if (position >= *bufused) {
        fprintf(stderr,"Writing out of buffer bounds!\n");
        exit(1);
    }
    if ((*bufsize - *bufused) < 1) {    //check free space in buffer
        increase_buffer(bufptr,bufsize);
    };
    memmove(buf + position + 1, buf + position, *bufused - position -1);
    buf[position] = chr;
    (*bufused)++;
}

void increase_buffer(char **bufptr, long *bufsize) {    //allocates more memory for buffer
    char *newbuf = realloc(*bufptr, *bufsize + BLOCK_SIZE);
    if (!newbuf) {
        fprintf(stderr,"Can't allocate %ld bytes of memory!\n", *bufsize + BLOCK_SIZE);
        exit(1);
    }
    else *bufptr = newbuf;
    *bufsize += BLOCK_SIZE;
}

long get_curs_pos(char screen_text[LINES][COLS+1]) {    //get current cursor position
    int x, y, i;
    long pos = 0;
    getyx(stdscr,y,x);
    for (i=0; i<y; i++) pos += strlen(screen_text[i]);
    pos += x;
    return pos;
}

long get_curs_pos_atxy(int x, int y, char screen_text[LINES][COLS+1]) { //get cursor position at given coordinates
    long pos = 0;
    int i;
    for (i=0; i<y; i++) pos += strlen(screen_text[i]);
    if (strlen(screen_text[y]) -1 >= x)
        pos += x;
    else
        pos += strlen(screen_text[y]) - 1;
    return pos;
}

void move_curs_to(long pos, char screen_text[LINES][COLS+1]) {      //move cursor to given position
    int i;
    long len;
    for (i=0; i<LINES; i++) {
        len = strlen(screen_text[i]);
        if (pos < len) {
            move(i,pos);
            break;
        }
        else
            pos -= len;
    }
}
