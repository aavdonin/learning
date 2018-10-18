#include <stdio.h>
#include <string.h>
#include "lib.h"
struct contact {char first_name[NAME_MAX_LEN]; char last_name[NAME_MAX_LEN]; int number;};
struct contact contacts[PHONEBOOK_CAPACITY];

void print_contact(struct contact c) {
  printf("First name: %s\nLast name: %s\nPhone number: %d\n\n",c.first_name,c.last_name,c.number);
};

void print_all() {
  int i;
  for(i=0;i<PHONEBOOK_CAPACITY;i++) {
    if (contacts[i].number != 0) 
      print_contact(contacts[i]);
    else {
      printf("End of list\n");
      break;
    };
  };
};

void clear_all() {
  int i;
  for(i=0;i<PHONEBOOK_CAPACITY;i++) {
    if (contacts[i].number != 0) {
      strcpy(contacts[i].first_name, "\0");
      strcpy(contacts[i].last_name, "\0");
      contacts[i].number = 0;
    }
    else {
      printf("Phonebook cleared!\n");
      break;
    };
  };
};

int add_contact(char *fname, char *lname, int number) {
  int i;
  for(i=0;i<PHONEBOOK_CAPACITY;i++) {
    if (contacts[i].number == 0) {
      strcpy(contacts[i].first_name, fname);
      strcpy(contacts[i].last_name, lname);
      contacts[i].number = number;
      break;
    };
  };
  if (i == PHONEBOOK_CAPACITY) {
    printf("Phonebook overfilled! Can't save contact.\n");
    return -1;
  };
  return i;
};

void search_by_number(int number) {
  int i, count = 0;
  for(i=0;i<PHONEBOOK_CAPACITY;i++) {
    if (contacts[i].number == number) {
      print_contact(contacts[i]);
      count++;
    }
    else if (contacts[i].number == 0)
      break;
  };
  if (count)
    printf("Found %d contact(s).\n",count);
  else
    printf("Not found.\n");
};

void search_by_name(char *name) {
  int i, count = 0;
  for(i=0;i<PHONEBOOK_CAPACITY;i++) {
    if (strstr(contacts[i].first_name, name) != NULL || strstr(contacts[i].last_name, name) != NULL ) {
      print_contact(contacts[i]);
      count++;
    }
    else if (contacts[i].number == 0)
      break;
  };
  if (count)
    printf("Found %d contact(s).\n",count);
  else
    printf("Not found.\n");
};