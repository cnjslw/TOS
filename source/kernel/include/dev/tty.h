/**
 * 终端tty
 */

#ifndef TTY_H
#define TTY_H

#include "ipc/mutex.h"

#define TTY_NR 8
#define TTY_IBUF_SIZE 512
#define TTY_OBUF_SIZE 512
typedef struct _tty_fifo_t {
    char* buf;
    int size; // 最大字节数
    int read, write; // 当前读写位置
    int count; // 当前已有的数据量
} tty_fifo_t;

typedef struct _tty_t {
    char obuf[TTY_OBUF_SIZE];
    tty_fifo_t ofifo; // 输出队列
    char ibuf[TTY_IBUF_SIZE];
    tty_fifo_t ififo; // 输入处理后的队列
    int console_idx; // 控制台索引号
} tty_t;

#endif