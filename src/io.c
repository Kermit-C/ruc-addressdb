#include <fcntl.h>
#include <unistd.h>

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
    if (1 && offset != 0)
    {
        // 0 的话还是顺序读
        lseek(db_fd, offset, 1);
    }
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
