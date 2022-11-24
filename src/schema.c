#include "schema.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io.h"

/**************** 模式相关 *******************/

/** 定义模式的每一个字段项 */
struct address_schema_field_item
{
    /**
     * 一些字段标志
     * * 第一位：为 0 则定长，为 1 则变长
     * * 第二、三位：00 为 int，01 为 float，10 为 char，11 为 varchar
     * * 后几位：保留标志
     */
    unsigned char sign;
    int length;                   // 字段长度，varchar 的实际长度由字段前 4 字节（int 长度）确定
    char name[field_name_length]; // 字段名
};

/** 通信簿用到的模式定义字段长度 */
int address_schema_length;

/** 通信簿用到的模式定义 */
struct address_schema_field_item *address_schema;

// /** 创建通信簿用到的模式定义 */
// void create_address_schema(const int address_schema_length)
// {
//     struct address_schema_field_item *new_address_schema = (struct address_schema_field_item *)malloc(address_schema_length * sizeof(struct address_schema_field_item));
//     // 第一个字段：定长 char 类型，长度 20 字节，表示名字
//     new_address_schema[0].sign = 0b01000000;
//     new_address_schema[0].length = 20;
//     strcpy(new_address_schema[0].name, "name");
//     // 第二个字段：定长 int 类型，长度 4 字节，表示年龄
//     new_address_schema[1].sign = 0b00000000;
//     new_address_schema[1].length = sizeof(int);
//     strcpy(new_address_schema[1].name, "age");
//     // 第三个字段：定长 char 类型，长度 20 字节，表示电话
//     new_address_schema[2].sign = 0b01000000;
//     new_address_schema[2].length = 40;
//     strcpy(new_address_schema[2].name, "phone");
//     // 第四个字段：定长 char 类型，长度 40 字节，表示 email
//     new_address_schema[3].sign = 0b01000000;
//     new_address_schema[3].length = 40;
//     strcpy(new_address_schema[3].name, "email");
//     // 第五个字段：定长 char 类型，长度 128 字节，表示“步骤二”的定长个人简历
//     new_address_schema[4].sign = 0b01000000;
//     new_address_schema[4].length = 128;
//     strcpy(new_address_schema[4].name, "resume");
//     // 第六个字段：不定长 varchar 类型，最长长度 1024 字节，表示“步骤三”的不定长自述
//     new_address_schema[5].sign = 0b01100000;
//     new_address_schema[5].length = 1024;
//     strcpy(new_address_schema[5].name, "description");

//     address_schema = new_address_schema;
// }

/** 从元数据获取通信簿用到的模式定义 */
void get_address_schema_from_metadata(char *meta_path)
{
    open_metadata(meta_path);                           // 打开元数据
    read_metadata(&address_schema_length, sizeof(int)); // 读取元数据的字段长度
    struct address_schema_field_item *new_address_schema = (struct address_schema_field_item *)malloc(address_schema_length * sizeof(struct address_schema_field_item));
    read_metadata((char *)new_address_schema, address_schema_length * sizeof(struct address_schema_field_item)); // 读取元数据
    address_schema = new_address_schema;
    close_metadata(); // 关闭元数据
}

/** 根据字段名获取字段项索引号 */
int get_address_schema_index_from_name(char *field_name)
{
    for (int index = 0; index < address_schema_length; index++)
    {
        if (strcmp(field_name, (address_schema + index)->name) == 0)
        {
            return index;
        }
    }
    return -1;
}

/** 根据索引号获取字段项定义 */
struct address_schema_field_item *get_address_schema_from_index(int index)
{
    if (index < 0 || index >= address_schema_length)
    {
        return NULL;
    }
    return (address_schema + index);
}

/** 根据字段名获取字段项定义 */
struct address_schema_field_item *get_address_schema_from_name(char *field_name)
{
    int index = get_address_schema_index_from_name(field_name);
    return get_address_schema_from_index(index);
}

/** 根据字段名获取字段项类型 */
int get_address_schema_type_from_name(char *field_name)
{
    struct address_schema_field_item *address_schema_item = get_address_schema_from_name(field_name);
    switch (((address_schema_item->sign >> 5) & 0b011))
    {
    case 0b000:
        // int 类型
        return 0;
    case 0b001:
        // float 类型
        return 1;
    case 0b010:
        // char 类型
        return 2;
    case 0b011:
        // varchar 类型
        return 3;
    default:
        return -1;
    }
}

/** 根据索引号获取字段名 */
char *get_address_schema_field_name_from_index(int index)
{
    return get_address_schema_from_index(index)->name;
}

/**************** 元组相关 *******************/

/**
 * 每个元组的结构如下：
 * - 长度(数据段): int 长度 4 Byte (一个元组最大 2^32 字节)
 * - 数据：不定长数据，具体由上方的模式确定
 */

/** 获取缓冲区应当的长度 */
int get_buf_length(int tuple_length)
{
    return tuple_length + sizeof(int);
}

/** 获取缓冲区表示长度的长度 */
int get_buf_tuple_length_length()
{
    return sizeof(int);
}

/** 从缓冲区获取元组长度 */
int get_tuple_length(unsigned char *buf)
{
    int length_pointer = *(int *)buf;
    return length_pointer;
}

/** 往缓冲区写元组长度 */
void set_tuple_length(unsigned char *buf, int length)
{
    *(int *)buf = length;
}

/** 从缓冲区获取数据段 */
unsigned char *get_tuple_data(unsigned char *buf)
{
    return (buf + sizeof(int));
}

/** 创建一个空元组缓冲区 */
unsigned char *create_one_buf()
{
    int tuple_length = 0; // 字节
    for (int index = 0; index < address_schema_length; index++)
    {
        tuple_length += (address_schema + index)->length;
    }
    return (unsigned char *)malloc(tuple_length);
}

/** 删除一个缓冲区 */
void delete_one_buf(unsigned char *buf)
{
    free(buf);
}

/** 从缓冲区获取字段值 */
void *get_tuple_data_field(unsigned char *buf, char *field_name)
{
    // 获取数据段
    unsigned char *tuple = get_tuple_data(buf);

    // 获取字段索引号
    int address_schema_index = get_address_schema_index_from_name(field_name);

    // 获取字段偏移量
    int field_offset = 0; // 字节
    struct address_schema_field_item *address_schema_item;
    for (int index = 0; index < address_schema_index; index++)
    {
        address_schema_item = get_address_schema_from_index(index);

        switch (((address_schema_item->sign >> 5) & 0b011))
        {
        case 0b000:
            // int 类型
            field_offset += address_schema_item->length;
            break;
        case 0b001:
            // float 类型
            field_offset += address_schema_item->length;
            break;
        case 0b010:
            // char 类型
            field_offset += address_schema_item->length;
            break;
        case 0b011:
            // carchar 类型
            field_offset += *(int *)(tuple + field_offset); // varchar 的实际长度由字段前 4 字节（int 长度）确定
            break;
        }
    }

    // 获取字段值
    address_schema_item = get_address_schema_from_index(address_schema_index);
    switch (((address_schema_item->sign >> 5) & 0b011))
    {
    case 0b000:
        // int 类型
        return (int *)(tuple + field_offset);
    case 0b001:
        // float 类型
        return (float *)(tuple + field_offset);
    case 0b010:
        // char 类型
        return (char *)(tuple + field_offset);
    case 0b011:
        // varchar 类型
        return (char *)(tuple + field_offset + sizeof(int)); // varchar 的实际长度由字段前 4 字节（int 长度）确定
    }
}

/** 往缓冲区写数据 */
int write_tuple(unsigned char *output_buf, int valuec, char **value)
{
    unsigned char *tuple = get_tuple_data(output_buf);
    int address_schema_index = 0;
    int address_offset = 0;
    struct address_schema_field_item *address_schema_item;
    for (int valuei = 0; valuei < valuec; valuei++)
    {
        address_schema_item = get_address_schema_from_index(address_schema_index);
        if (address_schema_item == NULL)
        {
            break;
        }

        char *curr_str = value[valuei];
        int len_curr_str;
        switch (((address_schema_item->sign >> 5) & 0b011))
        {
        case 0b000:
            // int 类型
            *(int *)(tuple + address_offset) = atoi(curr_str);
            address_offset += address_schema_item->length;
            break;
        case 0b001:
            // float 类型
            *(float *)(tuple + address_offset) = atof(curr_str);
            address_offset += address_schema_item->length;
            break;
        case 0b010:
            // char 类型
            len_curr_str = strlen(curr_str) + 1;
            if (len_curr_str > address_schema_item->length)
            {
                // 过长需要截断
                curr_str[address_schema_item->length - 1] = '\0';
                len_curr_str = address_schema_item->length;
            }
            strcpy((tuple + address_offset), curr_str);
            address_offset += address_schema_item->length;
            break;
        case 0b011:
            // varchar 类型
            len_curr_str = strlen(curr_str) + 1;
            if (len_curr_str > address_schema_item->length)
            {
                // 过长需要截断
                curr_str[address_schema_item->length - 1] = '\0';
                len_curr_str = address_schema_item->length;
            }

            *(int *)(tuple + address_offset) = len_curr_str;
            address_offset += sizeof(int);
            strcpy((tuple + address_offset), curr_str);
            address_offset += len_curr_str;
            break;
        }

        address_schema_index++;
    }
    set_tuple_length(output_buf, address_offset);

    return get_buf_length(address_offset);
}