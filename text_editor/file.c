#include <stdlib.h>
#include "defines.h"

long load_text(FILE *file, char **bufptr, long *bufused) {
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);
    long bsize = ((fsize/BLOCK_SIZE) + 1) * BLOCK_SIZE;
    char *b = malloc(bsize);
    if (!b) {
        fprintf(stderr,"Can't allocate %ld bytes of memory!\n", bsize);
        exit(1);
    }
    if (fread(b, 1, fsize, file) < fsize) {
        fprintf(stderr,"Something wrong while reading file contents...\n");
        fclose(file);
    }
    b[fsize] = '\0';
    *bufptr = b;
    *bufused = fsize+1;
    return bsize;
}

void save_text(FILE* file, char *buf, long bufused) {
    if (fwrite(buf, 1, bufused-1, file) < bufused-1) {
        fprintf(stderr,"Something wrong while writing file contents...\n");
        exit(1);
    }
}
