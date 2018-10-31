#include <stdio.h>

#define FILENAME_MAX_LEN 100
#define BLOCK_SIZE 262144

void print_menu(void);
long load_text(FILE*, char **bufptr, long *bufused);
void save_text(FILE*, char *buf, long bufused);
