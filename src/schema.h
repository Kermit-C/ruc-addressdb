#define field_name_length 20 // 字段名长度

/** 定义模式的每一个字段项 */
extern struct address_schema_field_item;

// /** 创建通信簿用到的模式定义 */
// void create_address_schema(const int address_schema_length);

/** 从元数据获取通信簿用到的模式定义 */
void get_address_schema_from_metadata(char *meta_path);

/** 根据字段名获取字段项索引号 */
int get_address_schema_index_from_name(char *field_name);

/** 根据索引号获取字段项定义 */
struct address_schema_field_item *get_address_schema_from_index(int index);

/** 根据字段名获取字段项定义 */
struct address_schema_field_item *get_address_schema_from_name(char *field_name);

/** 根据字段名获取字段项类型 */
int get_address_schema_type_from_name(char *field_name);

/** 根据索引号获取字段名 */
char *get_address_schema_field_name_from_index(int index);

/** 获取缓冲区应当的长度 */
int get_buf_length(int tuple_length);

/** 获取缓冲区表示长度的长度 */
int get_buf_tuple_length_length();

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
