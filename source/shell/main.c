/**
 * 简单的命令行解释器
 */

#include "main.h"
#include "fs/file.h"
#include "lib_syscall.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

static cli_t cli;
static const char* promot = "sh >>"; // 命令行提示符

/**
 * 显示命令行提示符
 */
static void show_promot(void)
{
    printf("%s", cli.promot);
    fflush(stdout);
}

/**
 * help命令
 */
static int do_help(int argc, char** argv)
{
    const cli_cmd_t* start = cli.cmd_start;

    // 循环打印名称及用法
    while (start < cli.cmd_end) {
        printf("%s %s\n", start->name, start->useage);
        start++;
    }
    return 0;
}

/**
 * @brief 清屏命令
 */
static int do_clear(int argc, char** argv)
{
    printf("%s", ESC_CLEAR_SCREEN);
    printf("%s", ESC_MOVE_CURSOR(0, 0));
    return 0;
}

/**
 * @brief 回显命令
 */
static int do_echo(int argc, char** argv)
{
    if (argc == 1) {
        char msg_buf[128];
        fgets(msg_buf, sizeof(msg_buf), stdin);
        msg_buf[sizeof(msg_buf) - 1] = '\0';
        puts(msg_buf);
        return 0;
    }

    int count = 1;
    int ch;
    /*
    这个函数调用是使用getopt函数来解析命令行参数。
    argc：表示命令行参数的数量，即argv数组的大小。
    argv：是一个指向字符串数组的指针，其中包含命令行参数的值。
    "n:h"：是一个字符串，用于指定程序所支持的选项。每个字符代表一个短选项。在这个例子中，"n:h"表示程序支持两个选项，即-n和-h。

    getopt函数将逐个检查命令行参数，并根据指定的选项规则进行解析。
    它会返回下一个找到的选项字符，并更新内部状态以跟踪已解析的选项和参数。

    返回值:
    如果找到下一个选项字符，则返回该字符。
    如果所有选项都已解析完毕，则返回-1。
    如果遇到未识别的选项字符，则返回?。
    并通过全局变量optarg保存当前选项的参数（如果有的话）。
    getopt(argc, argv, "n:h")用于解析命令行参数。
    它会循环调用getopt函数，每次迭代时都会处理下一个选项或参数，
    并根据不同的选项执行相应的操作。
    */
    while ((ch = getopt(argc, argv, "n:h")) != -1) {
        switch (ch) {
        case 'h':
            puts("echo echo any message");
            puts("Usage: echo [-n count] msg");
            optind = 1; // getopt需要多次调用，需要重置
            return 0;
        case 'n':
            count = atoi(optarg);
            break;
        case '?':
            if (optarg) {
                fprintf(stderr, "Unknown option: -%s\n", optarg);
            }
            optind = 1; // getopt需要多次调用，需要重置
            return -1;
        }
    }

    // 索引已经超过了最后一个参数的位置，意味着没有传入要发送的信息
    if (optind > argc - 1) {
        fprintf(stderr, "Message is empty \n");
        optind = 1; // getopt需要多次调用，需要重置
        return -1;
    }

    // 循环打印消息
    char* msg = argv[optind];
    for (int i = 0; i < count; i++) {
        puts(msg);
    }
    optind = 1; // getopt需要多次调用，需要重置
    return 0;
}

/**
 * @brief 程序退出命令
 */
static int do_exit(int argc, char** argv)
{
    exit(0);
    return 0;
}

/**
 * @brief 列出目录内容
 */
static int do_ls(int argc, char** argv)
{
    // 打开目录
    DIR* p_dir = opendir("temp");
    if (p_dir == NULL) {
        printf("open dir failed\n");
        return -1;
    }

    // 然后进行遍历
    struct dirent* entry;
    while ((entry = readdir(p_dir)) != NULL) {
        strlwr(entry->name);
        printf("%c %s %d\n",
            entry->type == FILE_DIR ? 'd' : 'f',
            entry->name,
            entry->size);
    }
    closedir(p_dir);

    return 0;
}

// 命令列表
static const cli_cmd_t cmd_list[] = {
    {
        .name = "help",
        .useage = "help -- list support command",
        .do_func = do_help,
    },
    {
        .name = "clear",
        .useage = "clear -- clear the screen",
        .do_func = do_clear,
    },
    {
        .name = "echo",
        .useage = "echo [-n count] msg -- echo something",
        .do_func = do_echo,
    },
    {
        .name = "ls",
        .useage = "ls -- list director",
        .do_func = do_ls,
    },
    {
        .name = "quit",
        .useage = "quit from shell",
        .do_func = do_exit,
    }
};

/**
 * 命令行初始化
 */
static void cli_init(const char* promot, const cli_cmd_t* cmd_list, int cnt)
{
    cli.promot = promot;

    memset(cli.curr_input, 0, CLI_INPUT_SIZE);

    cli.cmd_start = cmd_list;
    cli.cmd_end = cmd_list + cnt;
}

/**
 * 在内部命令中搜索
 */
static const cli_cmd_t* find_builtin(const char* name)
{
    for (const cli_cmd_t* cmd = cli.cmd_start; cmd < cli.cmd_end; cmd++) {
        if (strcmp(cmd->name, name) != 0) {
            continue;
        }

        return cmd;
    }

    return (const cli_cmd_t*)0;
}

/**
 * 运行内部命令
 */
static void run_builtin(const cli_cmd_t* cmd, int argc, char** argv)
{
    int ret = cmd->do_func(argc, argv);
    if (ret < 0) {
        fprintf(stderr, ESC_COLOR_ERROR "error: %d\n" ESC_COLOR_DEFAULT, ret);
    }
}

/**
 * @brief 运行当前文件
 */
static void run_exec_file(const char* path, int argc, char** argv)
{
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed: %s", path);
    } else if (pid == 0) {
        for (int i = 0; i < argc; i++) {
            msleep(1000);
            printf("arg %d = %s \n", i, argv[i]);
        }
        exit(-1);
    } else {
        int status;
        int pid = wait(&status);
        fprintf(stderr, "cmd %s result: %d, pid = %d\n", path, status, pid);
    }
}

int main(int argc, char** argv)
{
    open(argv[0], O_RDWR);
    dup(0); // 标准输出
    dup(0); // 标准错误输出

    puts("\033[31;42mWelcome to TOS : Base on X86\033[39;49m");
    printf("os version: %s\n", OS_VERSION);
    puts("author: AWei");
    puts("*****************************************");

    cli_init(promot, cmd_list, sizeof(cmd_list) / sizeof(cli_cmd_t));
    for (;;) {
        // 显示提示符，开始工作
        show_promot();

        // 获取输入的字符串，然后进行处理.
        // 注意，读取到的字符串结尾中会包含换行符和0
        char* str = fgets(cli.curr_input, CLI_INPUT_SIZE, stdin);
        if (str == (char*)0) {
            // 读不到错误，或f发生错误，则退出
            break;
        }

        // 读取的字符串中结尾可能有换行符，去掉之
        char* cr = strchr(cli.curr_input, '\n');
        if (cr) {
            *cr = '\0';
        }
        cr = strchr(cli.curr_input, '\r');
        if (cr) {
            *cr = '\0';
        }

        int argc = 0;
        char* argv[CLI_MAX_ARG_COUNT];
        memset(argv, 0, sizeof(argv));

        // 提取出命令，找命令表
        const char* space = " "; // 字符分割器
        char* token = strtok(cli.curr_input, space);
        while (token) {
            // 记录参数
            argv[argc++] = token;

            // 先获取下一位置
            token = strtok(NULL, space);
        }

        // 没有任何输入，则x继续循环
        if (argc == 0) {
            continue;
        }

        // 试图作为内部命令加载执行
        const cli_cmd_t* cmd = find_builtin(argv[0]);
        if (cmd) {
            run_builtin(cmd, argc, argv);
            continue;
        }
        // Test
        run_exec_file("", argc, argv);

        // 找不到命令，提示错误
        fprintf(stderr, ESC_COLOR_ERROR "Unknown command: %s\n" ESC_COLOR_DEFAULT, cli.curr_input);
        /*
        \x1b[31m 这个字符序列是用来控制终端输出文本颜色的 ANSI 转义序列。具体解释如下：
        \x1b：这是 ASCII 转义字符，表示将要开始一个转义序列。
        [：指定转义序列的开始。
        31：表示选择颜色的代码。在 ANSI 转义序列中，31对应红色。
        m：表示转义序列的结束。
        当终端遇到这个转义序列时，它会将后续的文本内容呈现为红色。
        在命令行中，如果你使用这个字符序列作为一部分的输出，终端会解析并根据其后的文本显示红色。例如：
        printf("\x1b[31mThis text is red.\x1b[0m");
        上述代码会将 "This text is red." 的文本显示为红色。\x1b[0m 是另一个转义序列，用于恢复默认的文本颜色和样式。
        */
    }

    return 0;
}