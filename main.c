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
      case '1': ;	//New contact	//semicolon to workaround the error: a label can only be part of a statement and a declaration is not a statement
        char fname[NAME_MAX_LEN];
        char lname[NAME_MAX_LEN];
        int number, id;
        printf("Adding new contact...\nEnter first name: ");
        scanf("%s", fname);
        printf("Enter last name: ");
        scanf("%s", lname);
        printf("Enter phone number: ");
        scanf("%d", &number);
        id = add_contact(fname,lname,number);
        if (id >= 0)
          printf("Successfully added contact #%d\n",++id);
        break;
      case '2':
        printf("Item 2\n");	//List all contacts
        print_all();
        break;
      case '3':
        printf("Item 3\n");	//Search by name
        break;
      case '4':
        printf("Item 4\n");	//Search by number
        break;
      case '5':
        printf("Item 5\n");	//Clear phonebook
        clear_all();
        break;
    };
  };
  printf("Good bye!\n");
  return 0;
};
