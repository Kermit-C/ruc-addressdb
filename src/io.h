
extern int db_fd;

/** 打开数据库文件文件描述符 */
int open_db(char *path);

/** 关闭数据库 */
int close_db();

/** 往缓冲区读取 */
int read_one(unsigned char *buf, int offset, int length);

/** 重置读指针偏移量 */
void reset_read_offset();

/** 往缓冲区写 */
int write_one(unsigned char *buf, int length);

extern int meta_fd;

/** 创建元数据 */
int create_metadata(char *path);

/** 打开元数据文件文件描述符 */
int open_metadata(char *path);

/** 关闭元数据 */
int close_metadata();

/** 读元数据 */
int read_metadata(char *buf, int length);

/** 写元数据 */
int write_metadata(char *buf, int length);
