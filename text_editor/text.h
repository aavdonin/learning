#ifndef _TEXT_H_
#define _TEXT_H_

void draw_screen(char *buf, char screen_text[LINES][COLS+1], long startpos);
void scroll_up(char *buf, char screen_text[LINES][COLS+1], long *startpos);
long get_curs_pos(char screen_text[LINES][COLS+1]);
long get_curs_pos_atxy(int x, int y, char screen_text[LINES][COLS+1]);
void move_curs_to(long pos, char screen_text[LINES][COLS+1]);
void del_chr(char *buf, long *bufused, long position);
void add_chr(char **bufptr, long *bufused, long *bufsize, long position, \
char chr);
void increase_buffer(char **bufptr, long *bufsize);

#endif