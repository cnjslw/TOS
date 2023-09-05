//
// https://wiki.osdev.org/Programmable_Interval_Timer
//

#include "dev/time.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "os_cfg.h"

static uint32_t sys_tick; // 系统启动后的tick数量

/**
 * 定时器中断处理函数
 */
void do_handler_timer(exception_frame_t* frame)
{
    sys_tick++;

    // 先发EOI，而不是放在最后
    // 放最后将从任务中切换出去之后，除非任务再切换回来才能继续
    // 硬件要求,每次中断处理之后需要发送eoi才能相应下一次中断
    pic_send_eoi(IRQ0_TIMER);
}

/**
 * 初始化硬件定时器
 */
static void init_pit(void)
{
    /*
    首先，计算每个时钟中断所需的计数器值。时钟频率为1193182 Hz，即每秒钟发生1193182次时钟脉冲。
    为了使中断每10毫秒发生一次，你需要计算出每10毫秒所需的时钟脉冲数。
    计算方法如下：
        时钟中断频率 = 1 / 中断间隔时间
        时钟中断频率 = 1 / 0.01  # 10毫秒等于0.01秒
        时钟中断频率 = 100  # 每秒钟100次中断
        每个时钟中断所需的计数器值 = 时钟频率 / 时钟中断频率
        每个时钟中断所需的计数器值 = 1193182 / 100
        每个时钟中断所需的计数器值 = 11931
    然后，你需要编写相应的代码将计数器值设置为11931，以使时钟中断每10毫秒产生一次。具体的实现方式取决于你使用的编程语言和操作系统。
    */
    uint32_t reload_count = PIT_OSC_FREQ / (1000.0 / OS_TICK_MS);

    outb(PIT_COMMAND_MODE_PORT, PIT_CHANNLE0 | PIT_LOAD_LOHI | PIT_MODE0);
    outb(PIT_CHANNEL0_DATA_PORT, reload_count & 0xFF); // 加载低8位
    outb(PIT_CHANNEL0_DATA_PORT, (reload_count >> 8) & 0xFF); // 再加载高8位

    irq_install(IRQ0_TIMER, (irq_handler_t)exception_handler_timer);
    irq_enable(IRQ0_TIMER);
}

/**
 * 定时器初始化
 */
void time_init(void)
{
    sys_tick = 0;
    init_pit();
}
