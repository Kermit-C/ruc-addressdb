#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"
#include "schema.h"
#include "operate.h"
#include "client.h"
#define MAX_COMMAND 1000 // 指令最长 1000 个字符

/** 客户端 worker */
void client_worker(char *path)
{
    // 打开数据库
    open_db(path);

    // 指令相关变量
    char input[MAX_COMMAND]; // 输入
    int input_index = 0;

    while (1)
    {
        memset(input, 0, sizeof(input));
        input_index = 0;
        printf("addressdb> ");
        gets(input); // 键入指令

        // 解析命令
        int stage = 0;
        char stage0[MAX_COMMAND];
        memset(stage0, 0, sizeof(stage0));
        char stage1[MAX_COMMAND];
        memset(stage1, 0, sizeof(stage1));
        char stage2[MAX_COMMAND];
        memset(stage2, 0, sizeof(stage2));
        char stage3[MAX_COMMAND];
        memset(stage3, 0, sizeof(stage3));
        int is_complete = 0; // 标识命令已正常完成

        unsigned char *last_pch = input - 1; // 上一个空格的指针
        unsigned char *curr_pch;             // 空格的指针
        while ((curr_pch = strstr(last_pch + 1, " ")) != NULL)
        {
            if (curr_pch - (last_pch + 1) == 0)
            {
                // 啥也不做，跳过多余空格
            }
            else if (stage == 0)
            {
                if (strncmp(last_pch + 1, "select", 6) == 0)
                    strncpy(stage0, last_pch + 1, 6);
                else if (strncmp(last_pch + 1, "insert", 6) == 0)
                    strncpy(stage0, last_pch + 1, 6);
                else if (strncmp(last_pch + 1, "help", 4) == 0)
                {
                    printf(
                        "Commands:\n"
                        "   insert value (value0,value1,...),(value0,value1,...),...\n"
                        "           Insert into address database.\n"
                        "   select field0,field1,... where field0=value0 and field1=value1 ...\n"
                        "           Find value from address database.\n"
                        "   help    Show usage.\n");
                    is_complete = 1;
                    break;
                }
                else
                {
                    printf("Command is not found. Input `help` to check available commands.\n");
                    is_complete = 1;
                    break;
                }
            }
            else if (stage == 1)
            {
                if (strcmp(stage0, "select") == 0)
                    strncpy(stage1, last_pch + 1, curr_pch - (last_pch + 1));
                else if (strcmp(stage0, "insert") == 0)
                {
                    if (strncmp(last_pch + 1, "value", 5) == 0)
                        strncpy(stage1, last_pch + 1, curr_pch - (last_pch + 1));
                    else
                        break;
                }
                else
                    break;
            }
            else if (stage == 2)
            {
                if (strcmp(stage0, "select") == 0)
                {
                    if (strncmp(last_pch + 1, "where", 5) == 0)
                        strncpy(stage2, last_pch + 1, curr_pch - (last_pch + 1));
                    else
                        break;
                }
                else if (strcmp(stage0, "insert") == 0)
                {
                    int n_insert = insert(last_pch + 1);
                    if (n_insert >= 0)
                    {
                        printf("%d item has been inserted.\n", n_insert);
                        is_complete = 1;
                        break;
                    }
                    else
                        break;
                }
                else
                    break;
            }
            else if (stage == 3)
            {
            }
            else
                break;

            last_pch = curr_pch;
            stage ++;
        };

        if (is_complete == 0)
        {
            show_syntax_error();
        }
    }

    close_db();
}

void show_syntax_error()
{
    printf("Syntax error.\n");
}
