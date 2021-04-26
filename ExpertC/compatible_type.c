#include <stdio.h>
/* Warning */
void foo(const char **p)
{
    printf("awful test\n");
}

/* NoWarning */
void correct(const char *p)
{
    printf("beautiful test\n");
}

int main(int argc, const char *argv[])
{
    char *cp = NULL;
    char **cpp = NULL;
    foo(cpp);
    correct(cp);
    return 0;
}
