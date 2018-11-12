#ifndef _UI_H_
#define _UI_H_

#include <ncurses.h>
void exit_failure(char *message);
void print_rec(WINDOW *winptr, struct file_rec rec);
void print_header(WINDOW *winptr);
void selection(WINDOW *winptr, int line, char enabled);
#endif
