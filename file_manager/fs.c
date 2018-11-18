#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "fs.h"
#include "ui.h"

int filter_func(const struct dirent *entry) {
    if (strcmp(entry->d_name,".") == 0)
        return 0;
    else if (strcmp(entry->d_name,"..") == 0) {
        char *path = getcwd(NULL,MAXPATH);
        if (strcmp(path,"/") == 0) { //root directory
            free(path);              //should not have
            return 0;                //".." record
        }
        else return 1;
    }
    else return 1;
}

struct file_rec get_rec(char *filename) {
    struct file_rec record;
    struct stat buffer;
    if (stat(filename, &buffer) < 0)
        exit_failure("Failed to get file size");
    strcpy(record.filename, filename);
    record.size = buffer.st_size;
    if (S_ISDIR(buffer.st_mode)) {
        record.type = '/';
        record.size = 0;
    }
    else if (buffer.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        record.type = '*';
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
    if (!info)
        exit_failure("Memory allocation failure");
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

void *copy(void *args) {
    //copies the file src to dest in separate thread
    cp_args *arg = (cp_args *)args;
    FILE *from, *to;
    from = fopen(arg->from, "r");
    to = fopen(arg->to, "w");
    char c;
    while ((c = fgetc(from)) != EOF) {
        fprintf(to, "%c", c);
    }
    fclose(from);
    fclose(to);
    int *result = malloc(sizeof(int));
    *result = 0;
    return (void *) result;
}
