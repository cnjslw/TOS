/**
 * 互斥锁
 */

#include "ipc/mutex.h"
#include "cpu/irq.h"

/**
 * @brief 锁初始化
 */
void mutex_init(mutex_t* mutex)
{
    mutex->locked_count = 0;
    mutex->owner = (task_t*)0;
    list_init(&mutex->wait_list);
}

/**
 * @brief 申请锁
 */
void mutex_lock(mutex_t* mutex)
{
    irq_state_t state = irq_enter_protection();

    task_t* curr = task_current();

    if (mutex->locked_count == 0) { // 没有任务获得了当前锁,直接占用
        mutex->locked_count = 1;
        mutex->owner = curr;
    } else if (mutex->owner == curr) { // 如果当前锁的拥有者是当前申请锁的任务,则锁计数加一
        mutex->locked_count++;
    } else { // 不是当前申请的任务,意味着已经有别的任务占用了当前的锁,所以直接将当前任务加入等待队列
        task_t* curr = task_current();
        task_set_block(curr);
        list_insert_last(&mutex->wait_list, &curr->wait_node);
        task_dispatch();
    }

    irq_leave_protection(state);
}

/**
 * @brief 释放锁
 */
void mutex_unlock(mutex_t* mutex)
{
    irq_state_t state = irq_enter_protection();

    task_t* curr = task_current();
    if (mutex->owner == curr) { // 只有拥有者才能释放锁
        if (--mutex->locked_count == 0) { // 互斥锁的计数为0才能释放
            mutex->owner = (task_t*)0;
            if (list_count(&mutex->wait_list)) { // 如果队列中有任务等待，则立即唤醒并占用锁
                list_node_t* task_node = list_remove_first(&mutex->wait_list);
                task_t* task = list_node_parent(task_node, task_t, wait_node);
                task_set_block(task);

                mutex->locked_count = 1;
                mutex->owner = task;

                task_dispatch();
            }
        }
    }

    irq_leave_protection(state);
}