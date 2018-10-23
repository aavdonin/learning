#include <stdio.h>
#include <dlfcn.h>
#include "lib.h"
#define LIB_COUNT 4

int main(void) {
  int menu_item;
  struct complex a, b;
  char *func_names[] = {"add","sub","mul","div",};
  char *lib_locs[] = {"plugin/libadd.so","plugin/libsub.so","plugin/libmul.so","plugin/libdiv.so",};
  void *libs[LIB_COUNT];
  struct complex (*funcs[LIB_COUNT])(struct complex, struct complex);
  int i;
  for (i=0;i<LIB_COUNT;i++) {   //open libs and get symbols for functions
      libs[i] = dlopen(lib_locs[i],RTLD_LAZY);
      funcs[i] = dlsym(libs[i],func_names[i]);
  };
  for (menu_item = -1;menu_item != '5';) {
    for (i=0;i<LIB_COUNT;i++) { //draw menu according to found symbols
      printf("%i)%s\n",i+1,funcs[i]?func_names[i]:"");
    };
    printf("5)quit\n");
    while ((menu_item = getchar()) == 10);
    if (menu_item < '1' || menu_item > '5') {	//validate input
      printf("You entered wrong number, try again...\n");
      continue;
    }
    else {
      menu_item = menu_item - '0';  // char '2' -> int 2, etc
    };
    if (menu_item == 5) break;
    else if (funcs[menu_item-1]) {  //if chosen function loaded
        get_numbers(&a,&b);
        print_result((*funcs[menu_item-1])(a,b));   //call it and print result
    };
  };
  return 0;
}
