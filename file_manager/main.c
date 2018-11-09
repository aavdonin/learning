#include <ncurses.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

struct file_rec {
    char type;
    char *filename;
    long size;
};

static int filter_func(const struct dirent *entry) {
    return strcmp(entry->d_name,".");
}

void exit_failure(char *message) {
    clear();
    printw("%s", message);
    getch();
    endwin();
    exit(1);
}

static struct file_rec get_rec(char *filename) {
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

void print_rec(struct file_rec rec) {
    printw("%c%-20s %.0ld", rec.type, rec.filename, rec.size);
}

int main(void) {
    int num, i;
    struct dirent **entry;
    struct file_rec record;
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    printw("Soon I'll become a file manager!\n");
    num = scandir(".", &entry, filter_func, alphasort);
    if (num<0) {
        exit_failure("Directory read error");
    }
    for (i = 0; i < num; i++) {
        record = get_rec(entry[i]->d_name);
        print_rec(record);
        printw("\n");
        free(entry[i]);
    }
    free(entry);

    refresh();
    getch();
    endwin();
    return 0;
}
