/**
 * 内核初始化以及测试代码
 *
 */
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include "core/memory.h"
#include "core/task.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "ipc/sem.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/list.h"
#include "tools/log.h"

static boot_info_t* init_boot_info; // 启动信息

/**
 * 内核入口
 */
void kernel_init(boot_info_t* boot_info)
{
    init_boot_info = boot_info;

    // 初始化CPU，再重新加载
    cpu_init();
    memory_init(boot_info);
    log_init();
    irq_init();
    time_init();

    task_manager_init();
}

// static task_t first_task; // 第一个任务
static uint32_t init_task_stack[1024]; // 空闲任务堆栈
static task_t init_task;
static sem_t sem;

/**
 * 初始任务函数
 * 目前暂时用函数表示，以后将会作为加载为进程
 */
void init_task_entry(void)
{
    int count = 0;

    for (;;) {
        sem_wait(&sem);
        log_printf("init task: %d", count++);
        //  sys_yield();
        sys_msleep(2000);
    }
}

/**
 * 链表测试
 */
void list_test()
{
    list_t list;
    list_node_t nodes[5];
    // 测试头插法
    list_init(&list);
    log_printf("list: first=0x%x, last=0x%x, count=%d", list_first(&list), list_last(&list), list_count(&list));
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        log_printf("insert first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_first(&list, node);
    }
    log_printf("list: first=0x%x, last=0x%x, count=%d", list_first(&list), list_last(&list), list_count(&list));
    // 测试尾插法
    list_init(&list);
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        log_printf("insert last to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_last(&list, node);
    }
    log_printf("list: first=0x%x, last=0x%x, count=%d", list_first(&list), list_last(&list), list_count(&list));

    // 测试删除头部节点
    for (int i = 0; i < 5; i++) {
        list_node_t* node = list_remove_first(&list);
        log_printf("remove first from list: %d, 0x%x", i, (uint32_t)node);
    }
    log_printf("list: first=0x%x, last=0x%x, count=%d", list_first(&list), list_last(&list), list_count(&list));

    // 插入
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        log_printf("insert last to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_last(&list, node);
    }
    log_printf("list: first=0x%x, last=0x%x, count=%d", list_first(&list), list_last(&list), list_count(&list));
    // 测试删除指定节点
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        log_printf("remove first from list: %d, 0x%x", i, (uint32_t)node);
        list_remove(&list, node);
    }
    log_printf("list: first=0x%x, last=0x%x, count=%d", list_first(&list), list_last(&list), list_count(&list));
    struct type_t {
        int i;
        list_node_t node;
    } v = { 0x123456 };

    list_node_t* v_node = &v.node;
    struct type_t* p = list_node_parent(v_node, struct type_t, node);
    if (p->i != 0x123456) {
        log_printf("error");
    }
}

void init_main(void)
{
    // ist_test();
    log_printf("Kernel is running....");
    log_printf("Version: %s, name: %s", OS_VERSION, "tiny x86 os");
    // log_printf("%d %d %x %c", -123, 123456, 0x12345, 'a');

    // 初始化任务
    task_init(&init_task, "init task", (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_first_init();
    // write_tr(first_task.tss_sel);

    // 信号量初始化
    sem_init(&sem, 2);

    // int a = 3 / 0;
    irq_enable_global();
    int count = 0;
    for (;;) {
        log_printf("first task: %d", count++);
        // sys_yield();
        sem_notify(&sem);
        sys_msleep(1000);
    }
}
