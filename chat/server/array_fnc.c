#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static void check_malloc(void *ptr) {
    if (ptr == NULL) {
        perror("Memory allocation failure");
        exit(1);
    }
}

int arrlen(int *arr) {
    int i = 0;
    while (arr && arr[i] != 0)
        i++;
    return i;
}

int *append(int *arr, int value) {
    int *newarr;
    if (!arr) {
        newarr = malloc(sizeof(int) * 2);
        check_malloc(newarr);
        newarr[0] = value;
        newarr[1] = 0;
    }
    else {
        int size = arrlen(arr);
        newarr = realloc(arr, sizeof(int) * (size+2));
        check_malloc(newarr);
        newarr[size] = value;
        newarr[size+1] = 0;
    }
    return newarr;
}

int *del(int *arr, int index) {
    int size = arrlen(arr);
    for (; index < size; index++) {
        arr[index] = arr[index+1];
    }
    int *newarr = realloc(arr, sizeof(int) * size);
    check_malloc(newarr);
    return newarr;
}
