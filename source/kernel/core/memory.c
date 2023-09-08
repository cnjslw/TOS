/**
 * 内存分配器
 */
#include "core/memory.h"
#include "tools/klib.h"
#include "tools/log.h"

static addr_alloc_t paddr_alloc; // 物理地址分配结构

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
 * @brief 显示硬件获取到的内存信息
 */
static void show_mem_info(boot_info_t* boot_info)
{
    log_printf("mem region (From Hardware Detected): ");
    for (int i = 0; i < boot_info->ram_region_count; i++) {
        log_printf("[%d]: 0x%x - 0x%x", i,
            boot_info->ram_region_cfg[i].start,
            boot_info->ram_region_cfg[i].size);
    }
    log_printf("\n");
}

/**
 * @brief 获取可用物理内存大小
 */
static uint32_t total_mem_size(boot_info_t* boot_info)
{
    int mem_size = 0;
    for (int i = 0; i < boot_info->ram_region_count; i++) {
        mem_size += boot_info->ram_region_cfg[i].size;
    }
    return mem_size;
}

/**
 * @brief 内存分配器初始化,boot_info是内核初始化时获得的内存信息
 * 1、初始化物理内存分配器：将所有物理内存管理起来. 在1MB内存中分配物理位图
 * 2、重新创建内核页表：原loader中创建的页表已经不再合适
 */
void memory_init(boot_info_t* boot_info)
{
    // 1MB以上归用户程序使用,1MB以下内核使用
    extern uint8_t* mem_free_start;
    log_printf("mem init.");
    show_mem_info(boot_info);

    // 在内核空间后面存放物理页位图
    uint8_t* mem_free = (uint8_t*)mem_free_start;

    // 计算1MB以上空间的空闲内存容量,并对齐页边界
    uint32_t mem_up1MB_free = total_mem_size(boot_info) - MEM_EXT_START;
    mem_up1MB_free = down2(mem_up1MB_free, MEM_PAGE_SIZE);
    log_printf("Free Memory : 0x%x , size : 0x%x", MEM_EXT_START, mem_up1MB_free);

    // 存放扩展内存的位图,比如4GB的内存,那么需要(4*2^30) / (4*2^10) / 8 = 128KB大小的位图
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_up1MB_free, MEM_PAGE_SIZE);
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);

    ASSERT(mem_free < (uint8_t*)MEM_EBDA_START);
}