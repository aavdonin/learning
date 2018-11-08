#ifndef _DEFINES_H_
#define _DEFINES_H_

#define FILENAME_MAX_LEN 100
#define BLOCK_SIZE 262144

void print_menu(void);
long load_text(FILE*, char **bufptr, long *bufused);
void save_text(FILE*, char *buf, long bufused);
void edit_mode(char **bufptr, long *startpos, long *bufused, long *bufsize);

#endif