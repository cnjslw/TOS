/**
 * 任务实现
 */
#ifndef TASK_H
#define TASK_H

#include "comm/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

#define TASK_NAME_SIZE 32 // 任务名字的长度
#define TASK_TIME_SLICE_DEFAULT 10 // 时间片计数
/**
 * @brief 任务控制块结构
 */
typedef struct _task_t {
    // uint32_t* stack; // 任务的栈
    enum { TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WAITING,
    } state;

    char name[TASK_NAME_SIZE]; // 任务的名字
    int time_slice; // 时间片
    int slice_ticks; // 递减时间片计数
    tss_t tss; // 任务的TSS段
    uint16_t tss_sel; // tss选择子
    list_node_t run_node; // 运行相关节点
    list_node_t all_node; // 所有队列节点
} task_t;

int task_init(task_t* task, const char* name, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t* from, task_t* to);
void task_set_ready(task_t* task);
void task_set_block(task_t* task);
int sys_yield(void);
void task_dispatch(void);
void task_time_tick(void);
task_t* task_current(void);
/**
 * @brief 任务管理器
 */

typedef struct _task_manager_t {
    task_t* curr_task;
    list_t ready_list;
    list_t task_list;
    task_t first_task;
} task_manager_t;

void task_manager_init(void);
void task_first_init(void);
task_t* task_first_task(void);

#endif