#ifndef __SEM_H__
#define __SEM_H__

#include "tools/list.h"

typedef struct _sem_t {
    int count; // 信号量计数
    list_t wait_list; // 等待的进程队列
} sem_t;

void sem_init(sem_t* sem, int init_count);
void sem_wait(sem_t* sem);
void sem_notify(sem_t* sem);
#endif