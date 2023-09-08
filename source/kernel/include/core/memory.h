#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "comm/boot_info.h"
#include "ipc/mutex.h"
#include "tools/bitmap.h"

#define MEM_EXT_START (1024 * 1024) //>=1MB
#define MEM_PAGE_SIZE 4096 //  页面大小
#define MEM_EBDA_START 0x00080000 // 供外设使用
/**
 * @brief 地址分配器结构
 */

typedef struct _addr_alloc_t {
    mutex_t mutex; // 地址分配器互斥信号量
    bitmap_t bitmap; // 辅助分配位图
    uint32_t page_size; // 该分配器分配的单位大小
    uint32_t start; // 起始地址
    uint32_t size; // 地址大小
} addr_alloc_t;

void memory_init(boot_info_t* boot_info);
#endif