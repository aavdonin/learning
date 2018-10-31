#include <stdlib.h>
#include "defines.h"

long load_text(FILE *file, char **bufptr) {
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);
    long bsize = ((fsize/BLOCK_SIZE) + 1) * BLOCK_SIZE;
    char *b = malloc(bsize);
    if (!b) {
        fprintf(stderr,"Can't allocate %ld bytes of memory!", bsize);
        exit(1);
    }
    if (fread(b, 1, fsize, file) < fsize) {
        fprintf(stderr,"Something wrong while reading file contents...\n");
        fclose(file);
    }
    b[fsize] = '\0';
    *bufptr = b;
    return bsize;
}
