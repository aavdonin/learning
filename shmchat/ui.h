#ifndef _UI_H_
#define _UI_H_

void init_screen(void);
struct sembuf get_sembuf(unsigned short sem_num, short sem_op, short sem_flg);
void *listenmsg(void *arg);
void clear_input(void);
void add_chr(int key, char *msg, int *pos);
void del_chr(char *msg, int *pos, int minpos);

#endif
