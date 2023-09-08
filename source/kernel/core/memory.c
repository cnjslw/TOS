/**
 * 内存分配器
 */

#include "core/memory.h"
#include "tools/klib.h"
#include "tools/log.h"

/**
 * @brief 初始化地址分配结构,检查start和size的页边界
 */
static void addr_alloc_init(addr_alloc_t* alloc, uint8_t* bits,
    uint32_t start, uint32_t size, uint32_t page_size)
{
    mutex_init(&alloc->mutex);
    alloc->start = start;
    alloc->size = size;
    alloc->page_size = page_size;
    // 初始化该区域的位图
    bitmap_init(&alloc->bitmap, bits, alloc->size / alloc->page_size, 0);
}

/**
 * @brief 分配多页内存
 */
static uint32_t addr_alloc_page(addr_alloc_t* alloc, int page_count)
{
    uint32_t addr = 0;

    mutex_lock(&alloc->mutex);

    int page_index = bitmap_alloc_nbits(&alloc->bitmap, 0, page_count);
    if (page_index >= 0) {
        addr = alloc->start + page_index * alloc->page_size;
    }

    mutex_unlock(&alloc->mutex);
    return addr;
}

/**
 * @brief 释放多页内存
 */
static void addr_free_page(addr_alloc_t* alloc, uint32_t addr, int page_count)
{
    mutex_lock(&alloc->mutex);

    uint32_t pg_idx = (addr - alloc->start) / alloc->page_size;
    bitmap_set_bit(&alloc->bitmap, pg_idx, page_count, 0);

    mutex_unlock(&alloc->mutex);
}

/**
 * @brief 内存分配器初始化,boot_info是内核初始化时获得的内存信息
 * 1、初始化物理内存分配器：将所有物理内存管理起来. 在1MB内存中分配物理位图
 * 2、重新创建内核页表：原loader中创建的页表已经不再合适
 */
void memory_init(boot_info_t* boot_info)
{
    addr_alloc_t addr_alloc;
    uint8_t bits[8];
    addr_alloc_init(&addr_alloc, bits, 0x10000, 64 * 4096, 4096);
    // 测试
    for (int i = 0; i < 32; i++) {
        uint32_t addr = addr_alloc_page(&addr_alloc, 2);
        log_printf("alloc addr: 0x%x", addr);
    }

    uint32_t addr = 0;
    for (int i = 0; i < 32; i++) {
        addr_free_page(&addr_alloc, addr, 2);
        addr += 8192;
    }
}