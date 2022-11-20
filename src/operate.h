#include "schema.h"

extern const int select_where_max_length; // 选择运算符最长

extern struct select_where_item;

/** 插入一条 */
void insert_one(int valuec, char **value);

/** insert 语句 */
int insert(char *syntax);

/** select 语句 */
int select_where(char *where_syntax, char *field_syntax);

/** select 语句是否匹配某个 where 项 */
int is_select_match(unsigned char *input_buf, char *field_name, char op, char *val);

/** 从缓冲区输出结果 */
void output_select_result(char *select_field[], int select_field_length, unsigned char *input_buf, int index);

/** 处理选择语句 */
int convert_select_syntax(char *where_syntax, char *field_syntax, struct select_where_item *select_where[], char *select_field[], int *select_where_length, int *select_field_length);
