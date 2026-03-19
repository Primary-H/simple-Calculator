#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==========================================
 * 1. 类型定义与全局配置 (Typedefs & Config)
 * ========================================== */
#define MAX_INPUT_LEN 1024

/* ==========================================
 * 2. 大数计算引擎 (Big Number Engine)
 * ========================================== */
// TODO: 实现 add_big_num, multiply_big_num 等

/* ==========================================
 * 3. 词法分析与调度场算法 (Lexer & Parser)
 * ========================================== */
// TODO: 实现 Token 解析和中缀转后缀逻辑

/* ==========================================
 * 4. 主程序入口 (Main)
 * ========================================== */
int main(int argc, char *argv[])
{
    // 模式 1: 命令行参数模式
    if (argc > 1)
    {
        printf("执行命令行模式，计算: %s\n", argv[1]);
        // TODO: 将 argv[1] 传入解析器
        return 0;
    }

    // 模式 2: 交互模式 (REPL)
    char input[MAX_INPUT_LEN];
    while (1)
    {
        // 读取输入
        if (fgets(input, MAX_INPUT_LEN, stdin) == NULL)
        {
            break;
        }

        // 去除末尾换行符
        input[strcspn(input, "\n")] = 0;

        // 退出条件
        if (strcmp(input, "quit") == 0)
        {
            break;
        }

        // 忽略空输入
        if (strlen(input) == 0)
        {
            continue;
        }

        // TODO: 将 input 传入解析器并打印结果
        printf("接收到输入: %s\n", input);
    }

    return 0;
}