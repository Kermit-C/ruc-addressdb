#include <fcntl.h>
#include <unistd.h>

/**************** 数据库相关 *******************/

int db_fd;

/** 打开数据库文件文件描述符 */
int open_db(char *path)
{
    if (access(path, F_OK) == 0)
    {
        // 文件存在
        db_fd = open(path, O_RDWR, 0);
        return db_fd;
    }
    else
    {
        // 文件不存在，以 755 权限创建
        db_fd = creat(path, 0755);
        close(db_fd);
        return open_db(path);
    }
}

/** 关闭数据库 */
int close_db()
{
    close(db_fd);
}

/** 往缓冲区读取 */
int read_one(unsigned char *buf, int offset, int length)
{
    lseek(db_fd, offset, 1);
    int n_read = read(db_fd, buf, length);
    return n_read;
}

/** 重置读指针偏移量 */
void reset_read_offset()
{
    lseek(db_fd, 0, 0);
}

/** 往缓冲区写 */
int write_one(unsigned char *buf, int length)
{
    lseek(db_fd, 0, 2);
    int n_write = write(db_fd, buf, length);
    return n_write;
}

/**************** 元数据相关 *******************/

int meta_fd;

/** 打开元数据文件文件描述符 */
int open_metadata(char *path)
{
    if (access(path, F_OK) == 0)
    {
        // 文件存在
        meta_fd = open(path, O_RDWR, 0);
        return meta_fd;
    }
    else
    {
        return -1;
    }
}

/** 创建元数据 */
int create_metadata(char *path)
{
    // 以 755 权限创建
    meta_fd = creat(path, 0755);
    close(meta_fd);
    return open_metadata(path);
}

/** 关闭元数据 */
int close_metadata()
{
    close(meta_fd);
}

/** 读元数据 */
int read_metadata(char *buf, int length)
{
    int n_read = read(meta_fd, buf, length);
    return n_read;
}

/** 写元数据 */
int write_metadata(char *buf, int length)
{
    lseek(meta_fd, 0, 2);
    int n_write = write(meta_fd, buf, length);
    return n_write;
}
