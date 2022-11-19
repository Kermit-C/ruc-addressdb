#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"
#include "schema.h"
#define MAX_COMMAND 1000 // 指令最长 1000 个字符

/** 客户端 worker */
void client_worker()
{

    // 指令相关变量
    char input[MAX_COMMAND + 11]; // 输入
    int input_index = 0;

    while (1)
    {
        memset(input, 0, sizeof(input));
        input_index = 0;
        printf("addressdb> ");
        gets(input); // 键入指令

        if (strncmp(input, "select ", 7))
        {
            /* code */
        }
        else if (strncmp(input, "insert value ", 13))
        {
            /* code */
        }

        else if (strncmp(input, "help", 4))
        {
            printf(
                "Commands:\n"
                "   insert value (value0,value1,...),(value0,value1,...),...\n"
                "           Insert into address database.\n"
                "   select field0,field1,... where field0=value0 and field1=value1 ...\n"
                "           Find value from address database.\n"
                "   help    Show usage.\n\n");
        }
        else
        {
            printf("Command is not found. Input `help` to check available commands.\n\n");
        }
    }
}
