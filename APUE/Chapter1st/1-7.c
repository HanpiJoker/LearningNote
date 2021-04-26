
#include "apue.h"
#include <sys/wait.h>

int main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;

    printf ("%% ");  /* print promt (printf requires %% to print %) */
    while (fgets (buf, MAXLINE, stdin) != NULL) {

        /* fgets()返回的每一行都是以换行符终止，后随一个null字节
         * 之所以需要将换行符用null字符替换掉，是因为execlp函数
         * 要求的参数是以null结束而不是换行符结束
         */
        if (buf[strlen(buf) - 1] == '\n') 
            buf[strlen(buf) - 1] = 0;  /* replace neline with null */
        if ((pid = fork ()) < 0) {
            err_sys ("fork error"); 
        } else if (pid == 0) {    /* child */
            execlp (buf, buf, (char *)0);
            err_ret ("couldn't execute: %s", buf);
            exit (127);
        }

        /* parent */
        if ((pid = waitpid (pid, &status, 0)) < 0)
            err_sys ("waitpid error");
        printf ("%% ");
    }
    exit (0);
}
