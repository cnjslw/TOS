#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"

#define CONSOLE_VIDEO_BASE 0xb8000
#define CONSOLE_DISP_ADDR 0xb8000 // 控制台显存起始地址,共32kb
#define CONSOLE_DISP_END (0xb8000 + 32 * 1024) // 显存的结束地址
#define CONSOLE_ROW_MAX 25 // 行数
#define CONSOLE_COL_MAX 80 // 最大列数

// 各种颜色
typedef enum _cclor_t {
    COLOR_Black = 0,
    COLOR_Blue = 1,
    COLOR_Green = 2,
    COLOR_Cyan = 3,
    COLOR_Red = 4,
    COLOR_Magenta = 5,
    COLOR_Brown = 6,
    COLOR_Gray = 7,
    COLOR_Dark_Gray = 8,
    COLOR_Light_Blue = 9,
    COLOR_Light_Green = 10,
    COLOR_Light_Cyan = 11,
    COLOR_Light_Red = 12,
    COLOR_Light_Magenta = 13,
    COLOR_Yellow = 14,
    COLOR_White = 15
} cclor_t;

/**
 * @brief 显示字符
 */
typedef union {
    struct {
        char c; // 显示字符
        char foreground : 4; // 前景色
        char background : 3; // 背景色
    };
    uint16_t v;
} disp_char_t;

/**
 * @brief 终端描述符,描述当前字符的位置,背景色,字体色
 */
typedef struct _console_t {
    disp_char_t* disp_base; // 显示基地址
    int cursor_row, cursor_col; // 当前编辑的行与列
    int display_rows, display_cols; // 显示界面的行数和列数
    cclor_t foreground, background; // 前后景色
} console_t;

int console_init(void);
int console_write(int dev, char* data, int size);
void console_close(int dev);

#endif