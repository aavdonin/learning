#include <stdio.h>

int main() {
  char buf[30];
  fgets(buf,2,stdin);
  printf(">%s<\n",buf);
  printf("Maybe there's also smth in buffer: >%c<\n",getchar());
};