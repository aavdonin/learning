#ifndef _FS_H_
#define _FS_H_
#include <dirent.h>

struct file_rec {
    char type;
    char *filename;
    long size;
};

int filter_func(const struct dirent *entry);
struct file_rec get_rec(char *filename);
#endif
