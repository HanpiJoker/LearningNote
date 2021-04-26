#include "apue.h"
/* int e = 9; */
/* int g; */

int main(int argc, const char* argv[])
{
    int i;
    /* g = 10; */

    /* for (i = 0; i < argc; i++) */
    for (i = 0; argv[i] != NULL; i++)
        printf("argv[%d]: %s\n", i, argv[i]);
    /* printf("%s\n", *environ); */
    /* printf("%d\n\n%d\n", e, g); */
    exit(0);
}
