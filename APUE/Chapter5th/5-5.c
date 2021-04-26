#include "apue.h"

int main(void)
{
    char buf[MAXLINE];

    while (fgets(buf, MAXLINE, stdin) != NULL)
        if (fputs(buf, stdout) == EOF)
            err_sys("output error");

    if (ferror(stdin))
        err_sys("input error");

    exit(0);
}

/*
 * 
 * brief: main 
 * The code of test 2;
 * return: 
 *
 * #define MAX 4
 * int main(void)
 * {
 *     char buf[MAX];
 * 
 *     while (fgets(buf, MAX, stdin) != NULL)
 *         if (fputs (buf, stdout) == EOF)
 *             err_sys ("output error");
 * 
 *     if (ferror (stdin))
 *         err_sys ("input error");
 * 
 *     exit (0);
 * }
 */
