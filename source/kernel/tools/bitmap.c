/**
 * 内存分配的位图操作
 */
#include "tools/bitmap.h"
#include "tools/klib.h"
/**
 * @brief 位图初始化
 */
void bitmap_init(bitmap_t* bitmap, uint8_t* bits, int count, int init_bits)
{
    bitmap->bit_count = count;
    bitmap->bits = bits;
    int bytes = bitmap_byte_count(bitmap->bit_count);
    kernel_memset(bitmap->bits, init_bits ? 0xFF : 0, bytes);
}

/**
 * @brief 位图计数器
 */
int bitmap_byte_count(int bit_count)
{
    return (bit_count + 8 - 1) / 8;
}

/**
 * @brief 获取指定位置的状态
 */
int bitmap_get_bit(bitmap_t* bitmap, int index)
{
    return bitmap->bits[index / 8] & (1 << (index % 8));
}

/**
 * @brief 设置连续个位,从index开始的位置,设置连续count个位置为bit
 */
void bitmap_set_bit(bitmap_t* bitmap, int index, int count, int bit)
{
    for (int i = 0; (i < count) && (index < bitmap->bit_count); i++, index++) {
        if (bit) {
            bitmap->bits[index / 8] |= 1 << (index % 8);
        } else {
            bitmap->bits[index / 8] &= ~(1 << (index % 8));
        }
    }
}

/**
 * @brief 检查位是否为1(该页是否分配)
 */
int bitmap_is_set(bitmap_t* bitmap, int index)
{
    return bitmap_get_bit(bitmap, index) ? 1 : 0;
}

/**
 * @brief 寻找为位图中,连续count个值为bit的索引,返回第一个位的索引
 */
int bitmap_alloc_nbits(bitmap_t* bitmap, int bit, int count)
{
    int search_idx = 0;
    int ok_idx = -1;
    while (search_idx < bitmap->bit_count) {
        if (bitmap_get_bit(bitmap, search_idx) != bit) {
            search_idx++;
            continue;
        }
        ok_idx = search_idx;
        int i;
        for (i = 1; (i < count) && (search_idx < bitmap->bit_count); i++) {
            if (bitmap_get_bit(bitmap, search_idx++) != bit) {
                ok_idx = -1;
                break;
            }
        }

        if (i >= count) {
            bitmap_set_bit(bitmap, ok_idx, count, ~bit);
            return ok_idx;
        }
    }
}