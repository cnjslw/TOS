/**
 * 任务管理
 *
 */
#include "core/task.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"

static task_manager_t task_manager;
static int tss_init(task_t* task, uint32_t entry, uint32_t esp)
{
    // 为TSS分配GDT
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0) {
        log_printf("alloc tss failed.\n");
        return -1;
    }

    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
        SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);

    // tss段初始化
    kernel_memset(&task->tss, 0, sizeof(tss_t));
    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.eip = entry;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss.es = task->tss.ss = task->tss.ds
        = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_DS; // 暂时写死
    task->tss.cs = KERNEL_SELECTOR_CS; // 暂时写死
    task->tss.iomap = 0;

    task->tss_sel = tss_sel;
    return 0;
}

/**
 * @brief 初始化任务
 */
int task_init(task_t* task, const char* name, uint32_t entry, uint32_t esp)
{
    ASSERT(task != (task_t*)0);
    // uint32_t* pesp = (uint32_t*)esp;
    // if (esp) {
    //     *(--pesp) = entry;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     task->stack = pesp;
    // }
    int err = tss_init(task, entry, esp);
    if (err < 0) {
        log_printf("init task failed.\n");
        return err;
    }
    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    task->state = TASK_CREATED;
    task->time_slice = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_slice;
    list_node_init(&task->all_node);
    list_node_init(&task->run_node);
    irq_state_t state = irq_enter_protection();
    task_set_ready(task);
    list_insert_last(&task_manager.task_list, &task->all_node);
    irq_leave_protection(state);
    return 0;
}

/**
 * @brief 抛弃繁重的TSS,手动切换任务
 */
void simple_switch(uint32_t** from, uint32_t* to);

/**
 * @brief 切换至指定任务
 */
void task_switch_from_to(task_t* from, task_t* to)
{
    switch_to_tss(to->tss_sel);
    // simple_switch(&from->stack, to->stack);
}
/**
 * @brief 任务管理器初始化
 */
void task_manager_init(void)
{
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    task_manager.curr_task = (task_t*)0;
}

/**
 * @brief 第一个任务初始化,类似linux的init线程
 */
void task_first_init(void)
{
    task_init(&task_manager.first_task, "first task", 0, 0);
    write_tr(task_manager.first_task.tss_sel);
    task_manager.curr_task = &task_manager.first_task;
}

/**
 * @brief 返回初始化任务
 */
task_t* task_first_task(void)
{
    return &task_manager.first_task;
}

void task_set_ready(task_t* task)
{
    list_insert_last(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}
/**
 * @brief 将任务从就绪队列中删除
 */
void task_set_block(task_t* task)
{
    list_remove(&task_manager.ready_list, &task->run_node);
}

/**
 * @brief 获取下一个即将运行的任务
 */
static task_t* task_next_run(void)
{
    list_node_t* task_node = list_first(&task_manager.ready_list);
    return list_node_parent(task_node, task_t, run_node);
}

/**
 * @brief 获取当前正在运行的任务
 */
task_t* task_current(void)
{
    return task_manager.curr_task;
}

/**
 * @brief 主动让出CPU
 */
int sys_yield(void)
{
    irq_state_t state = irq_enter_protection();
    if (list_count(&task_manager.ready_list) > 1) {
        task_t* curr_task = task_current();
        // 将当前任务从Ready队列头部取出,放到队列最后
        task_set_block(curr_task);
        task_set_ready(curr_task);
        // 切换任务
        task_dispatch();
    }
    irq_leave_protection(state);
    return 0;
}

/**
 * @brief 任务调度
 */
void task_dispatch(void)
{
    task_t* to = task_next_run();
    if (to != task_manager.curr_task) {
        task_t* from = task_manager.curr_task;
        task_manager.curr_task = to;

        to->state = TASK_RUNNING;
        task_switch_from_to(from, to);
    }
}

/**
 * @brief 时间处理
 */
void task_time_tick(void)
{
    task_t* curr_task = task_current();
    irq_state_t state = irq_enter_protection();
    // 时间片处理
    if (--curr_task->slice_ticks == 0) {
        // 时间片用完,重置时间片
        curr_task->slice_ticks = curr_task->time_slice;
        task_set_block(curr_task);
        task_set_ready(curr_task);
        task_dispatch();
    }
    irq_leave_protection(state);
}