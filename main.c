#include <stdio.h>
#include "lib.h"

int main(void) {
  int menu_item;
  printf("Hello. I am a simple phonebook program. You can enter your contacts here, ");
  printf("list them, search by number and name, delete, etc.\n\nSo, what do you want to do?\n");
  for (menu_item = -1;menu_item != '0';) {
    printf("Enter number:\n1 - New contact\n2 - List all contacts\n3 - Search by name\n");
    printf("4 - Search by number\n5 - Clear phonebook\n\n0 - Exit\n");
    while ((menu_item = getchar()) == 10);
    if (menu_item < '0' || menu_item > '5') {	//validate input
      printf("You entered wrong number, try again...\n");
      continue;
    };
    switch (menu_item) {	//do what it says
      case '1':
        printf("Item 1\n");	//New contact
        break;
      case '2':
        printf("Item 2\n");	//List all contacts
        break;
      case '3':
        printf("Item 3\n");	//Search by name
        break;
      case '4':
        printf("Item 4\n");	//Search by number
        break;
      case '5':
        printf("Item 5\n");	//Clear phonebook
        break;
    };
  };
  printf("Good bye!\n");
  return 0;
};
