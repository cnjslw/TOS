/**
 * 控制台设备
 * 支持VGA
 */

#include "dev/console.h"
#define CONSOLE_NR 1 // 控制台数量
static console_t console_buf[CONSOLE_NR];

/**
 * @brief 光标前移一个字符
 */
static void move_forward(console_t* console, int n)
{
    for (int i = 0; i < n; i++) {
        if (++console->cursor_col >= console->display_cols) {
            console->cursor_col = 0;
            console->cursor_row++;
        }
    }
}

/**
 * @brief 在当前位置显示一个字符
 */
static void show_char(console_t* console, char c)
{
    int offset = console->cursor_col + console->cursor_row * console->display_cols;
    disp_char_t* p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_forward(console, 1);
}

/**
 * @brief 清除console
 */
static void clear_display(console_t* console)
{
    int size = console->display_cols * console->display_rows;
    disp_char_t* start = console->disp_base;
    for (int i = 0; i < size; i++, start++) {
        start->c = ' ';
        start->foreground = console->foreground;
        start->background = console->background;
    }
}

/**
 * @brief 控制台初始化
 */
int console_init(void)
{
    // 初始化字符显示的属性
    for (int i = 0; i < CONSOLE_NR; i++) {
        console_t* console = console_buf + i;
        console->disp_base = (disp_char_t*)CONSOLE_DISP_ADDR;
        console->display_cols = CONSOLE_COL_MAX;
        console->display_rows = CONSOLE_ROW_MAX;
        console->cursor_row = 0;
        console->cursor_col = 0;
        console->foreground = COLOR_White;
        console->background = COLOR_Black;
        clear_display(console);
    }
    return 0;
}

/**
 * @brief 向控制台写
 */
int console_write(int dev, char* data, int size)
{
    console_t* console = console_buf + dev;
    int len;
    for (int i = 0; len < size; len++) {
        char c = *data++;
        show_char(console, c);
    }
    return len;
}

/**
 * @brief 关闭控制台
 */
void console_close(int dev) { }