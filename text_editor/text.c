#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"

void draw_screen(char *buf, char screen_text[LINES][COLS+1], long startpos);
long get_curs_pos(char screen_text[LINES][COLS+1]);
long get_curs_pos_atxy(int x, int y, char screen_text[LINES][COLS+1]);
void move_curs_to(long pos, char screen_text[LINES][COLS+1]);

void draw_screen(char *buf, char screen_text[LINES][COLS+1], long startpos) {
    clear();
    int i;
    int eof = 0;
    for (i=0; i<LINES; i++) {
        int j = 0;
        while (j < COLS) {
            screen_text[i][j] = buf[startpos + j];
            if (buf[startpos + j] == '\0') break;
            if ((buf[startpos + j] == '\n') && ((COLS - j) > 1)) {
                screen_text[i][++j] = '\0';
                break;
            }
/*            if (buf[startpos + j] == '\t') {
                if ((COLS - j) >= 4) {
                    int k;
                    for (k=0;k<4;k++) {
                        screen_text[i][j+k] = ' ';
                    }
                }
            }
*/
            j++;
            if (j == COLS) screen_text[i][j] = '\0';
        }
        startpos += j;
        printw("%s",screen_text[i]);
    }
    refresh();
}

void edit_mode(char *buf, long *startpos) {
    int key = KEY_UP;
    int x, y;   //coordinates for cursor positioning
    char screen_text[LINES][COLS+1];
    long curs_pos = 0;  //position of cursor
    draw_screen(buf, screen_text, *startpos);
    move(0,0);
    //curs_pos = get_curs_pos(screen_text);
    curs_pos = 0;
    while ((key = getch()) != 27) { //While pressed key is not ESC
        switch (key) {
            case KEY_UP:
                getyx(stdscr,y,x);
                curs_pos = get_curs_pos_atxy(x, y-1, screen_text);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_DOWN:
                getyx(stdscr,y,x);
                curs_pos = get_curs_pos_atxy(x, y+1, screen_text);
                move_curs_to(curs_pos, screen_text);
                break;
            case KEY_LEFT:
                move_curs_to(--curs_pos, screen_text);
                break;
            case KEY_RIGHT:
                move_curs_to(++curs_pos, screen_text);
                break;
        }
    }
}

void del_chr(char *buf, long *bufused, long position) {
    if (position >= *bufused) {
        fprintf(stderr,"Erasing out of buffer bounds!\n");
        exit(1);
    }
    memmove(buf + position, buf + position + 1, *bufused - position);
    (*bufused)--;
}

void add_chr(char **bufptr, long *bufused, long *bufsize, long *position, char chr) {
    char *buf = *bufptr;
    if (*position >= *bufused) {
        fprintf(stderr,"Writing out of buffer bounds!\n");
        exit(1);
    }
    if ((*bufsize - *bufused) < 1) {    //check free space in buffer
        increase_buffer(bufptr,bufsize);
    };
    memmove(buf + *position + 1, buf + *position, *bufused - *position -1);
    buf[*position] = chr;
    (*bufused)++;
}

void increase_buffer(char **bufptr, long *bufsize) {
    char *newbuf = realloc(*bufptr, *bufsize + BLOCK_SIZE);
    if (!newbuf) {
        fprintf(stderr,"Can't allocate %ld bytes of memory!\n", *bufsize + BLOCK_SIZE);
        exit(1);
    }
    else *bufptr = newbuf;
    *bufsize += BLOCK_SIZE;
}

long get_curs_pos(char screen_text[LINES][COLS+1]) {
    int x, y, i;
    long pos = 0;
    getyx(stdscr,y,x);
    for (i=0; i<y; i++) pos += strlen(screen_text[i]);
    pos += x;
    return pos;
}

long get_curs_pos_atxy(int x, int y, char screen_text[LINES][COLS+1]) {
    long pos = 0;
    int i;
    for (i=0; i<y; i++) pos += strlen(screen_text[i]);
    pos += x;
    return pos;
}

void move_curs_to(long pos, char screen_text[LINES][COLS+1]) {
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
