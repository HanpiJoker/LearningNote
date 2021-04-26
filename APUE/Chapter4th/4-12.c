#include "apue.h"

int main(void)
{
    struct stat statbuf;

    /* turn on set-group-ID and turn off group-execute */

    if (stat ("foo", &statbuf) < 0)
        err_sys ("stat error for foo");
    if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        err_sys ("chmod error for foo");

    /* set absolute mode to "rw-r--r--" */

    if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
        err_sys ("chmod error for bar");
    exit (0);
}

/*
 * 代码分析:
 * 值得注意的是, ls命令列出的时间和日期没有变化。是因为chmod()函数只更新i-node最近一次更改的时间,
 * 按系统默认方式, ls -l显示的是最后修改文件内容的时间。
 */
