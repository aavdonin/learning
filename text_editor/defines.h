#include <stdio.h>

#define FILENAME_MAX_LEN 100
#define BLOCK_SIZE 262144

void print_menu(void);
long load_text(FILE*, char **bufptr, long *bufused);
void save_text(FILE*, char *buf, long bufused);

void draw_screen(char *buf, long startpos);
void edit_mode(char *buf, long *startpos);
void del_chr(char *buf, long *bufused, long position);
void add_chr(char **bufptr, long *bufused, long *bufsize, long *position, char chr);
void increase_buffer(char **bufptr, long *bufsize);
