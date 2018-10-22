#define PHONEBOOK_CAPACITY 1000
#define NAME_MAX_LEN 30

int add_contact(char *first_name, char *last_name, int number);
void print_all();
void clear_all();
void search_by_number(int num);
void search_by_name(char *name);
