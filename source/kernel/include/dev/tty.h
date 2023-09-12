/**
 * 终端tty
 */

#ifndef TTY_H
#define TTY_H

#include "ipc/sem.h"

#define TTY_NR 8
#define TTY_IBUF_SIZE 512
#define TTY_OBUF_SIZE 512
#define TTY_OCRLF (1 << 0)

typedef struct _tty_fifo_t {
    char* buf;
    int size; // 最大字节数
    int read, write; // 当前读写位置
    int count; // 当前已有的数据量
} tty_fifo_t;

int tty_fifo_get(tty_fifo_t* fifo, char* c);
int tty_fifo_put(tty_fifo_t* fifo, char c);

typedef struct _tty_t {
    char obuf[TTY_OBUF_SIZE];
    tty_fifo_t ofifo; // 输出队列
    sem_t osem;
    char ibuf[TTY_IBUF_SIZE];
    tty_fifo_t ififo; // 输入处理后的队列
    int oflags;
    int console_idx; // 控制台索引号
} tty_t;

#endif