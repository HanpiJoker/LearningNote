/*
 * BUFFSIZE是一个自定义的常量
 * 而buf[BUFFSIZE]是一个自定义的缓冲区
 */
#include "apue.h"

#define BUFFSIZE 4096

int main(void)
{
    int n;
    char buf[BUFFSIZE];
    
    /* 
     * 两个常量 STDIN_FILENO 和 STDOUT_FILENO
     * 定义在<unistd.h>头文件中，它们指定了标准输入
     * 和标准输出的文件描述符
     */
    while ((n = read (STDIN_FILENO, buf, BUFFSIZE)) > 0)
        if (write (STDOUT_FILENO, buf, n) != n)
            err_sys ("write error");

    if (n < 0)
        err_sys ("read error");

    exit (0);
}
