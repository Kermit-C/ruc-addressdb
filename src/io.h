
extern int db_fd;

/** 打开数据库文件文件描述符 */
int open_db(char *path);

/** 关闭数据库 */
int close_db();

/** 往缓冲区读取 */
int read_one(unsigned char *buf, int offset, int length);

/** 往缓冲区写 */
int write_one(unsigned char *buf, int offset, int length);
