/**
 * 日志输出
 */
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include <stdarg.h>

// 目标用串口，参考资料：https://wiki.osdev.org/Serial_Ports
#define COM1_PORT 0x3F8 // RS232端口0初始化

/**
 * @brief 初始化日志输出
 */
void log_init(void)
{
    // 利用qemu的串行接口显示字符
    outb(COM1_PORT + 1, 0x00); // Disable all interrupts
    outb(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1_PORT + 1, 0x00); //                  (hi byte)
    outb(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(COM1_PORT + 4, 0x0F);
}

/**
 * @brief 日志打印
 */
void log_printf(const char* fmt, ...)
{
    char str_buf[128];
    va_list args;

    kernel_memset(str_buf, '\0', sizeof(str_buf));

    va_start(args, fmt);
    kernel_vsprintf(str_buf, fmt, args);
    va_end(args);

    const char* p = str_buf;
    while (*p != '\0') {
        while ((inb(COM1_PORT + 5) & (1 << 6)) == 0) // 检查串口忙状态
            ;
        outb(COM1_PORT, *p++);
    }

    outb(COM1_PORT, '\r');//处理ASCII码中特殊字符
    outb(COM1_PORT, '\n');
}
