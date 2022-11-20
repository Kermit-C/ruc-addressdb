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
    int length;    // 字段长度，varchar 的实际长度由字段前 4 字节（int 长度）确定
    char name[20]; // 字段名，20 字节
};

extern const int address_schema_length;

/** 通信簿用到的模式定义 */
extern const struct address_schema_field_item address_schema[];

/** 根据字段名获取字段项索引号 */
int get_address_schema_index_from_name(char *field_name);

/** 根据索引号获取字段项定义 */
struct address_schema_field_item *get_address_schema_from_index(int index);

/** 根据字段名获取字段项定义 */
struct address_schema_field_item *get_address_schema_from_name(char *field_name);

/** 从缓冲区获取元组长度 */
int get_tuple_length(unsigned char *buf);

/** 往缓冲区写元组长度 */
void set_tuple_length(unsigned char *buf, int length);

/** 从缓冲区获取数据段 */
unsigned char *get_tuple_data(unsigned char *buf);

/** 创建一个空元组缓冲区 */
unsigned char *create_one_buf();

/** 删除一个缓冲区 */
void delete_one_buf(unsigned char *buf);

/** 从缓冲区获取字段值 */
void *get_tuple_data_field(unsigned char *buf, char *field_name);

/** 往缓冲区写数据 */
int write_tuple(unsigned char *output_buf, int valuec, char **value);
