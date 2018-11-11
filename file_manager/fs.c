#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "fs.h"
#include "ui.h"

int filter_func(const struct dirent *entry) {
    return strcmp(entry->d_name,".");
}

struct file_rec get_rec(char *filename) {
    struct file_rec record;
    struct stat buffer;
    if (stat(filename, &buffer) < 0)
        exit_failure("Failed to get file size");
    record.filename = filename;
    record.size = buffer.st_size;
    if (S_ISDIR(buffer.st_mode)) {
        record.type = '/';
        record.size = 0;
    }
    else
        record.type = ' ';
    return record;
}

int get_dir_info(char *path, struct file_rec **records) {
    int num, i;
    struct dirent **entry;
    num = scandir(path, &entry, filter_func, alphasort);
    if (num<0) {
        exit_failure("Directory read error");
    }
    struct file_rec *info = malloc(sizeof(struct file_rec) * num);
    for (i = 0; i < num; i++) {
        info[i] = get_rec(entry[i]->d_name);
        free(entry[i]);
    }
    free(entry);
    if (*records)
        free(*records);
    *records = info;
    return num;
}
