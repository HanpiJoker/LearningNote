#include "apue.h"

void my_setbuf(FILE* fp, char* buf);
void pr_stdio(const char*, FILE*);
int is_unbuffered(FILE*);
int is_linebuffered(FILE*);
int buffer_size(FILE*);

int main(void)
{
    char buf[BUFSIZ];
    FILE* fp;

    if ((fp = fopen("tempfile", "a+")) == NULL)
        err_sys("fopen eror");

    my_setbuf(stdin, buf);
    pr_stdio("stdin", stdin);

    my_setbuf(stdout, buf);
    pr_stdio("stdout", stdout);

    my_setbuf(stderr, buf);
    pr_stdio("stderr", stderr);

    my_setbuf(fp, buf);
    pr_stdio("tempfile", fp);

    my_setbuf(fp, NULL);
    pr_stdio("tempfile", fp);

    fclose(fp);

    exit(0);
}
void pr_stdio(const char* name, FILE* fp)
{
    printf("stream = %s, ", name);
    if (is_unbuffered(fp))
        printf("unbuffered");
    else if (is_linebuffered(fp))
        printf("line buffered");
    else /* if neither of above */
        printf("fully buffered");
    printf(", buffer size = %d\n", buffer_size(fp));
}

int is_unbuffered(FILE* fp)
{
    return (fp->_flags & _IO_UNBUFFERED);
}

int is_linebuffered(FILE* fp)
{
    return (fp->_flags & _IO_LINE_BUF);
}

int buffer_size(FILE* fp)
{
    return (fp->_IO_buf_end - fp->_IO_buf_base);
}

void my_setbuf(FILE* fp, char* buf)
{
    if (buf == NULL) {
        if (setvbuf(fp, buf, _IONBF, BUFSIZ))
            err_sys("setvbuf error");
    } else {
        if (fp == stderr) {
            if (setvbuf(fp, buf, _IONBF, BUFSIZ))
                err_sys("setvbuf error");
        } else if (fp == stdin || fp == stdout) {
            if (setvbuf(fp, buf, _IOLBF, BUFSIZ))
                err_sys("setvbuf error");
        } else {
            if (setvbuf(fp, buf, _IOFBF, BUFSIZ))
                err_sys("setvbuf error");
        }
    }
}
