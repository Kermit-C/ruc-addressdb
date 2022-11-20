#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"
#include "schema.h"

/** 插入一条 */
void insert_one(int valuec, char **value)
{
    unsigned char *output_buf = create_one_buf();            // 创建单个大小缓冲区
    int buf_length = write_tuple(output_buf, valuec, value); // 写缓冲区
    write_one(output_buf, buf_length);                       // IO
    delete_one_buf(output_buf);                              // 释放缓冲区
}

/** insert */
int insert(char *syntax)
{
    int n_insert = 0;

    // 解析 field
    unsigned char *item_first = syntax - 1; // 一条的起始位置
    unsigned char *item_last = syntax - 1;  // 一条的结束位置，也作为分割不同条的分割点判断标准
    while (1)
    {
        if ((item_first = strstr(item_last + 1, "(")) == NULL)
        {
            return n_insert; // 完成插入
        }
        else if ((item_last = strstr(item_first + 1, ")")) == NULL)
        {
            return -1;
        }

        char *value[address_schema_length]; // 存储插入数据的数组
        int valuec = 0;

        // 构造插入数据的数组
        unsigned char *last_field_pch = item_first; // 上一个逗号的指针
        unsigned char *curr_field_pch;              // 逗号的指针
        while ((curr_field_pch = strstr(last_field_pch + 1, ",")) != NULL || (curr_field_pch = strstr(last_field_pch + 1, ")")) != NULL)
        {
            char *item_value = (char *)malloc(curr_field_pch - (last_field_pch + 1) + 1);
            memset(item_value, 0, sizeof(item_value));
            strncpy(item_value, last_field_pch + 1, curr_field_pch - (last_field_pch + 1));
            value[valuec++] = item_value;
            last_field_pch = curr_field_pch;
        }

        // 插入
        if (address_schema_length != valuec)
        {
            return -1; // 字段数不一样
        }
        insert_one(valuec, value);

        // 释放内存
        for (int index = 0; index < valuec; index++)
        {
            free(value[index]);
        }

        n_insert++;
    }
}