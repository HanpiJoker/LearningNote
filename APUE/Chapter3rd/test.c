/*
 * 编写一个函数与dup2()功能相同。要求不调用fcntl()函数。
 * 思路: 功能与dup2()函数功能相同, 即为复制文件描述符。首先想到的是fcntl();
 * 不过由于题目要求不能使用fcntl(), 因此可以使用的只剩下了dup()函数。如此一来, 
 * 主要思路就是不断调用dup()函数, 不断返回fd, 直至fd等于fd2。
 */

#include "apue.h"
#include <fcntl.h>

#define BUFFSIZE 10

typedef struct FD_List
{
    int myfd;
    struct FD_List *Next;
} MyFD_List;

int my_dup2 (int , int);

int main(void)
{
    int fdin;
    int fdout;
    int n;

    char buffer[BUFFSIZE];
    
    fdin = my_dup2 (STDIN_FILENO, 4);
    fdout = my_dup2 (STDOUT_FILENO, 6);

    printf ("STDIN fd = %d\n", fdin);
    printf ("STDOUT fd = %d\n", fdout);

    while ((n = read(fdin, buffer, BUFFSIZE)) > 0)
    {
        if (write(fdout, buffer, n) != n)
            err_sys ("write error");
    }

    if (n < 0)
        printf ("read error");
    exit (0);
}

int my_dup2 (int fd, int fd2)
{
    int tmpfd;

    MyFD_List *prev = NULL;
    MyFD_List *p;
    MyFD_List *temp;

    close (fd2);

    while ((tmpfd = dup(fd)) != fd2)
    {
        p = (MyFD_List *)malloc(sizeof(MyFD_List));
        p->myfd = tmpfd;
        p->Next = prev;
        prev = p; 
    }
    
    while (prev != NULL)
    {
        close (prev->myfd);
        temp = prev;
        prev = prev->Next;
        free (temp);
    }
    
    return fd2;
}
