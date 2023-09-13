/**
 * 命令行实现
 */
#ifndef CMD_H
#define CMD_H

#define CLI_INPUT_SIZE 1024 // 输入缓存
#define CLI_MAX_ARG_COUNT 10 // 最大输入参数

#define ESC_CMD2(Pn, cmd)		    "\x1b["#Pn#cmd
#define	ESC_COLOR_ERROR			    ESC_CMD2(31, m)	// 红色错误
#define	ESC_COLOR_DEFAULT		    ESC_CMD2(39, m)	// 默认颜色
#define ESC_CLEAR_SCREEN		    ESC_CMD2(2, J)	// 擦除整屏幕
#define	ESC_MOVE_CURSOR(row, col)  "\x1b["#row";"#col"H"

/**
 * @brief 具体命令的名称,使用方法
 */
typedef struct _cli_cmd_t {
    const char* name; // 命令名称
    const char* useage; // 使用方法
    int (*do_func)(int argc, char** argv); // 回调函数
} cli_cmd_t;

/**
 * @brief 命令管理器
 */
typedef struct _cli_t {
    char curr_input[CLI_INPUT_SIZE]; // 输入缓存
    const cli_cmd_t* cmd_start; // 命令起
    const cli_cmd_t* cmd_end; // 命令终
    const char* promot; // TOS>>
} cli_t;

#endif