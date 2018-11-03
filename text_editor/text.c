#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"

void draw_screen(char *buf, long startpos) {
    clear();
    int i;
    int eof = 0;
    char screen_text[LINES][COLS+1];
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
    draw_screen(buf, *startpos);
    while ((key = getch()) != 27) { //While pressed key is not ESC

    }
}

void del_chr(char *buf, long *bufused, long position) {
    if (position >= *bufused) {
        fprintf(stderr,"Erasing out of buffer bounds!\n");
        exit(1);
    }
    memcpy(buf + position, buf + position + 1, *bufused - position);
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
