#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
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
