#ifndef _FS_H_
#define _FS_H_
#include <dirent.h>
#include "defines.h"

struct file_rec {
    char type;
    char filename[MAXFILE];
    long size;
};

typedef struct {
    char from[MAXPATH];
    char to[MAXPATH];
} cp_args;

int filter_func(const struct dirent *entry);
struct file_rec get_rec(char *filename);
int get_dir_info(char *path, struct file_rec **records);
void *copy(void *);
#endif
