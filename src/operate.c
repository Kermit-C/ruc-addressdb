#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"
#include "schema.h"
#include "operate.h"

extern const int address_schema_length;

const int select_where_max_length = 1024; // 选择运算符最长 1024

/** 选择运算项 */
struct select_where_item
{
    char field_name[field_name_length]; // 字段名
    /**
     * 运算符
     * * 0b00000000: =
     * * 0b00000001: !=
     * * 0b00000010: >
     * * 0b00000011: <
     * * 0b00000100: >=
     * * 0b00000101: <=
     */
    unsigned char op;
    char *val; // 值的指针
};

/** 插入一条 */
void insert_one(int valuec, char **value)
{
    unsigned char *output_buf = create_one_buf();            // 创建单个大小缓冲区
    int buf_length = write_tuple(output_buf, valuec, value); // 写缓冲区
    write_one(output_buf, buf_length);                       // IO
    delete_one_buf(output_buf);                              // 释放缓冲区
}

/** insert 语句 */
int insert(char *syntax)
{
    int n_insert = 0;

    // 解析 field
    strcat(syntax, " ");
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

/** select 语句 */
int select_where(char *where_syntax, char *field_syntax)
{
    int n_select = 0;

    // 处理后存在这里
    struct select_where_item *select_where[select_where_max_length]; // 选择运算项
    int select_where_length;                                         // 选择运算项个数
    char *select_field[address_schema_length];                       // 字段选择项
    int select_field_length;                                         // 字段选择项个数

    // 解析选择语句
    if (convert_select_syntax(where_syntax, field_syntax, select_where, select_field, &select_where_length, &select_field_length) < 0)
    {
        // 处理失败的情况
        return -1;
    }

    // 读取
    unsigned char *input_buf = create_one_buf();                   // 创建单个大小缓冲区
    reset_read_offset();                                           // 数据库读指针归 0
    const int tuple_length_length = get_buf_tuple_length_length(); // 当前元组长度的长度
    int tuple_length = 0;                                          // 当前元组长度
    while (1)
    {
        int n_read_tuple_length = read_one((unsigned char *)&tuple_length, 0, tuple_length_length); // 读取当前元组长度
        if (n_read_tuple_length < tuple_length_length)
            break;
        read_one(input_buf, -tuple_length_length, tuple_length_length + tuple_length); // 读取到缓冲区

        // 匹配元组
        int is_match = 1;                // 是否匹配 where
        int curr_select_where_index = 0; // 当前处理的选择运算项索引项
        for (; curr_select_where_index < select_where_length; curr_select_where_index++)
        {
            char *field_name = select_where[curr_select_where_index]->field_name;
            char op = select_where[curr_select_where_index]->op;
            char *val = select_where[curr_select_where_index]->val;

            // 筛选、输出
            if (is_select_match(input_buf, field_name, op, val) == 0)
            {
                // 不匹配
                is_match = 0;
                break;
            }
        }

        // 匹配后输出
        if (is_match == 1)
        {
            n_select++;
            output_select_result(select_field, select_field_length, input_buf, n_select);
        }
    }
    printf("-------------------------\n");
    delete_one_buf(input_buf); // 释放缓冲区

    return n_select;
}

/** select 语句是否匹配某个 where 项 */
int is_select_match(unsigned char *input_buf, char *field_name, char op, char *val)
{
    int field_type = get_address_schema_type_from_name(field_name); // 获取字段类型

    // 筛选
    if (field_type == 0) // int
    {
        int converted_val = atoi(val);
        int data_val = *(int *)get_tuple_data_field(input_buf, field_name); // 获取 field
        switch (op)
        {
        case 0b00000000:
            // =
            if (data_val == converted_val)
            {
                return 1;
            }
            break;
        case 0b00000001:
            // !=
            if (data_val != converted_val)
            {
                return 1;
            }
            break;
        case 0b00000010:
            // >
            if (data_val > converted_val)
            {
                return 1;
            }
            break;
        case 0b00000011:
            // <
            if (data_val < converted_val)
            {
                return 1;
            }
            break;
        case 0b00000100:
            // >=
            if (data_val >= converted_val)
            {
                return 1;
            }
            break;
        case 0b00000101:
            // <=
            if (data_val <= converted_val)
            {
                return 1;
            }
            break;
        default:
            break;
        }
    }
    else if (field_type == 1) // float
    {
        float converted_val = atof(val);
        float data_val = *(float *)get_tuple_data_field(input_buf, field_name); // 获取 field
        switch (op)
        {
        case 0b00000000:
            // =
            if (data_val == converted_val)
            {
                return 1;
            }
            break;
        case 0b00000001:
            // !=
            if (data_val != converted_val)
            {
                return 1;
            }
            break;
        case 0b00000010:
            // >
            if (data_val > converted_val)
            {
                return 1;
            }
            break;
        case 0b00000011:
            // <
            if (data_val < converted_val)
            {
                return 1;
            }
            break;
        case 0b00000100:
            // >=
            if (data_val >= converted_val)
            {
                return 1;
            }
            break;
        case 0b00000101:
            // <=
            if (data_val <= converted_val)
            {
                return 1;
            }
            break;
        default:
            break;
        }
    }
    else if (field_type == 2 || field_type == 3) // char, varchar
    {
        char *data_val = (char *)get_tuple_data_field(input_buf, field_name); // 获取 field
        switch (op)
        {
        case 0b00000000:
            // =
            if (strcmp(data_val, val) == 0)
            {
                return 1;
            }
            break;
        case 0b00000001:
            // !=
            if (strcmp(data_val, val) != 0)
            {
                return 1;
            }
            break;
        case 0b00000010:
            // >
            if (strcmp(data_val, val) > 0)
            {
                return 1;
            }
            break;
        case 0b00000011:
            // <
            if (strcmp(data_val, val) < 0)
            {
                return 1;
            }
            break;
        case 0b00000100:
            // >=
            if (strcmp(data_val, val) >= 0)
            {
                return 1;
            }
            break;
        case 0b00000101:
            // <=
            if (strcmp(data_val, val) <= 0)
            {
                return 1;
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

/** 从缓冲区输出结果 */
void output_select_result(char *select_field[], int select_field_length, unsigned char *input_buf, int index)
{
    printf("-----------(%d)-----------\n", index);
    for (int select_field_index = 0; select_field_index < select_field_length; select_field_index++)
    {
        char *field_name = select_field[select_field_index];
        int field_type = get_address_schema_type_from_name(field_name); // 获取字段类型

        if (field_type == 0) // int
        {
            int data_val = *(int *)get_tuple_data_field(input_buf, field_name); // 获取 field
            printf("%s: %d\n", field_name, data_val);
        }
        else if (field_type == 1) // float
        {
            float data_val = *(float *)get_tuple_data_field(input_buf, field_name); // 获取 field
            printf("%s: %f\n", field_name, data_val);
        }
        else if (field_type == 2 || field_type == 3) // char, varchar
        {
            char *data_val = (char *)get_tuple_data_field(input_buf, field_name); // 获取 field
            printf("%s: %s\n", field_name, data_val);
        }
    }
}

/** 处理选择语句 */
int convert_select_syntax(char *where_syntax, char *field_syntax, struct select_where_item *select_where[], char *select_field[], int *select_where_length, int *select_field_length)
{
    memset(select_where, 0, select_where_max_length * sizeof(struct select_where_item *));
    memset(select_field, 0, address_schema_length * sizeof(char *));

    // 处理 where 参数
    strcat(where_syntax, " ");
    strcat(field_syntax, " ");
    unsigned char *last_where_pch = where_syntax - 1; // 上一个 and 的指针
    unsigned char *curr_where_pch;                    //  and 的指针
    int curr_select_where_index = 0;                  // 当前运算符项索引号
    while ((curr_where_pch = strstr(last_where_pch + 1, " ")) != NULL)
    {
        if (curr_where_pch - (last_where_pch + 1) == 0)
        {
            // 跳过多余空格
            last_where_pch = curr_where_pch;
            continue;
        }

        if (strncmp(last_where_pch + 1, "and", 3) == 0)
        {
            // and 连接符处理
            last_where_pch = curr_where_pch;
            continue;
        }

        // 处理运算符
        select_where[curr_select_where_index] = (struct select_where_item *)malloc(sizeof(struct select_where_item));
        unsigned char *op_pch; // 操作符的指针
        int op_length;         // 操作符长度
        if (((op_pch = strstr(last_where_pch + 1, "!=")) != NULL) && op_pch < curr_where_pch)
        {
            select_where[curr_select_where_index]->op = 0b00000001;
            op_length = 2;
        }
        else if (((op_pch = strstr(last_where_pch + 1, ">=")) != NULL) && op_pch < curr_where_pch)
        {
            select_where[curr_select_where_index]->op = 0b00000100;
            op_length = 2;
        }
        else if (((op_pch = strstr(last_where_pch + 1, "<=")) != NULL) && op_pch < curr_where_pch)
        {
            select_where[curr_select_where_index]->op = 0b00000101;
            op_length = 2;
        }
        else if (((op_pch = strstr(last_where_pch + 1, "=")) != NULL) && op_pch < curr_where_pch)
        {
            select_where[curr_select_where_index]->op = 0b00000000;
            op_length = 1;
        }
        else if (((op_pch = strstr(last_where_pch + 1, ">")) != NULL) && op_pch < curr_where_pch)
        {
            select_where[curr_select_where_index]->op = 0b00000010;
            op_length = 1;
        }
        else if (((op_pch = strstr(last_where_pch + 1, "<")) != NULL) && op_pch < curr_where_pch)
        {
            select_where[curr_select_where_index]->op = 0b00000011;
            op_length = 1;
        }
        else
        {
            return -1;
        }

        // 处理选择字段名
        memset(select_where[curr_select_where_index]->field_name, 0, field_name_length);
        strncpy(select_where[curr_select_where_index]->field_name, last_where_pch + 1, op_pch - (last_where_pch + 1));

        // 处理选择值
        select_where[curr_select_where_index]->val = (char *)malloc(curr_where_pch - (op_pch + op_length) + 1);
        memset(select_where[curr_select_where_index]->val, 0, curr_where_pch - (op_pch + op_length) + 1);
        strncpy(select_where[curr_select_where_index]->val, op_pch + op_length, curr_where_pch - (op_pch + op_length));

        curr_select_where_index++;

        last_where_pch = curr_where_pch;
    }
    *select_where_length = curr_select_where_index;

    // 处理 field 参数
    unsigned char *last_field_pch = field_syntax - 1; // 上一个逗号的指针
    unsigned char *curr_field_pch;                    // 逗号的指针
    int curr_select_field_index = 0;                  // 当前索引号
    while ((curr_field_pch = strstr(last_field_pch + 1, ",")) != NULL || (curr_field_pch = strstr(last_field_pch + 1, " ")) != NULL)
    {
        if (curr_field_pch - (last_field_pch + 1) == 1 && *(last_field_pch + 1) == '*')
        {
            // 使用 select * where ...
            char *whole_field_syntax = (char *)malloc((field_name_length + 1) * address_schema_length);
            memset(whole_field_syntax, 0, (field_name_length + 1) * address_schema_length);
            for (int address_schema_index = 0; address_schema_index < address_schema_length; address_schema_index++)
            {
                strcat(whole_field_syntax, get_address_schema_field_name_from_index(address_schema_index));
                if (address_schema_index < address_schema_length - 1)
                    strcat(whole_field_syntax, ",");
            }

            last_field_pch = whole_field_syntax - 1;
            continue;
        }

        char *item_value = (char *)malloc(curr_field_pch - (last_field_pch + 1) + 1);
        memset(item_value, 0, sizeof(item_value));
        strncpy(item_value, last_field_pch + 1, curr_field_pch - (last_field_pch + 1));
        select_field[curr_select_field_index++] = item_value;

        last_field_pch = curr_field_pch;
    }
    *select_field_length = curr_select_field_index;

    return 0;
}
