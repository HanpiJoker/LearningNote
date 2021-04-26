#include "apue.h"
#include <sys/utsname.h>

int main(void)
{
    struct utsname* name;

    if (uname(name) == -1)
        err_sys("uname error");
    printf("%s\n", name->sysname);
    printf("%s\n", name->nodename);
    printf("%s\n", name->release);
    printf("%s\n", name->version);
    printf("%s\n", name->machine);

    exit(0);
}
