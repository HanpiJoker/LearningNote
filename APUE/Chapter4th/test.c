/* 
 * 函数目的:
 * 简单实现cp(1)命令的功能, 遇到空洞文件, 跳过空洞实现
 */
#include "apue.h"
#include <unistd.h>
#include <fcntl.h>

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
#define BUFFSIZE 4096

int mycopy(int fd, int fd2) {
    int n;
    char buf;

    while ((n = read (fd, &buf, 1)) > 0)
    {
        if (buf == '\0')
           continue;
        if (write (fd2, &buf, 1) != 1)
           err_sys ("write error"); 
    }
    if (n < 0)
        err_sys ("read error");
    close (fd);
    close (fd2);
    return 0;
}

int main(int argc, char *argv[])
{
    char *src_path;
    char *dest_path;
    int fd, fd2;

    if (argc != 3)
        err_sys ("usage: mycopy <source path> <dest path>");
    
    src_path = argv[1];
    dest_path = argv[2];

    if ((fd = open (src_path, O_RDWR)) < 0)
        err_sys ("open error");
    if ((fd2 = open (dest_path, O_RDWR | O_CREAT, RWRWRW)) < 0)
        err_sys ("open error");
    if (mycopy (fd, fd2) == 0)
        printf ("copy successfully\n"); 
    exit (0);
}
