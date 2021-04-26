#include <stdio.h>
int main(void)
{
    int z, y = 1, x = 2;
    z = y+++x;
    printf("z = y+++x z:%d y:%d x:%d\n", z, y, x);
    printf("z = y+++++x compiler error\n");
    return 0;
}
