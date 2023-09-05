﻿/**
 * 内核初始化以及测试代码
 */
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"

static boot_info_t* init_boot_info; // 启动信息

/**
 * 内核入口
 */
void kernel_init(boot_info_t* boot_info)
{
    init_boot_info = boot_info;

    cpu_init();
    log_init();
    irq_init();
    time_init();
}

void init_main(void)
{
    log_printf("Kernel is running....");
    log_printf("Version: %s, name: %s", OS_VERSION, "TOS : Tiny Operating System work on X86");
    log_printf("%d %d %x %c", -123, 123456, 0x12345, 'a');

    int a = 3;
    ASSERT(a > 2);
    ASSERT(a < 2);

    // int a = 3 / 0;
    // irq_enable_global();
    for (;;) { }
}
