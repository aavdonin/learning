#ifndef _FS_H_
#define _FS_H_
#include <dirent.h>

struct file_rec {
    char type;
    char filename[255];
    long size;
};

int filter_func(const struct dirent *entry);
struct file_rec get_rec(char *filename);
int get_dir_info(char *path, struct file_rec **records);
#endif
