#include <stdio.h>
#include <dlfcn.h>
#include "lib.h"

int main(void) {
  int menu_item;
  struct complex a, b;
  //list plugin folder ??
  //load existing libs
  void *libadd, *libsub, *libmul, *libdiv;
  libadd = dlopen("plugin/libadd.so",RTLD_LAZY);
  if (!libadd) fprintf(stderr, "dlopen error: %s\n", dlerror());
  libsub = dlopen("plugin/libsub.so",RTLD_LAZY);
  if (!libsub) fprintf(stderr, "dlopen error: %s\n", dlerror());
  libmul = dlopen("plugin/libmul.so",RTLD_LAZY);
  if (!libmul) fprintf(stderr, "dlopen error: %s\n", dlerror());
  libdiv = dlopen("plugin/libdiv.so",RTLD_LAZY);
  if (!libdiv) fprintf(stderr, "dlopen error: %s\n", dlerror());
  //get symbols for operation funcs
  struct complex (*add)(struct complex, struct complex);
  struct complex (*sub)(struct complex, struct complex);
  struct complex (*mul)(struct complex, struct complex);
  struct complex (*div)(struct complex, struct complex);
  add = dlsym(libadd,"add");
  if (!add) fprintf(stderr, "dlsym error: %s\n", dlerror());
  sub = dlsym(libsub,"sub");
  if (!sub) fprintf(stderr, "dlsym error: %s\n", dlerror());
  mul = dlsym(libmul,"mul");
  if (!mul) fprintf(stderr, "dlsym error: %s\n", dlerror());
  div = dlsym(libdiv,"div");
  if (!div) fprintf(stderr, "dlsym error: %s\n", dlerror());
  //draw menu according to found symbols
  for (menu_item = -1;menu_item != '5';) {
    printf("1)%s\n2)%s\n3)%s\n4)%s\n5)Quit\n",add?"Add":"",sub?"Sub":"",mul?"Mul":"",div?"Div":"");
    while ((menu_item = getchar()) == 10);
    if (menu_item < '1' || menu_item > '5') {	//validate input
      printf("You entered wrong number, try again...\n");
      continue;
    };
    if (menu_item == '5') break;
    switch (menu_item) {	//do what it says
      case '1': ;	//Add
        if (add) {
            get_numbers(&a,&b);
            print_result((*add)(a,b));
        }
        break;
      case '2':         //Sub
        if (sub) {
            get_numbers(&a,&b);
            print_result((*sub)(a,b));
        }
        break;
      case '3':         //Mul
        if (mul) {
            get_numbers(&a,&b);
            print_result((*mul)(a,b));
        }
        break;
      case '4':         //Div
        if (div) {
            get_numbers(&a,&b);
            print_result((*div)(a,b));
        }
        break;
    };
  };
  return 0;
}
