#include <stdio.h>
int main(void)
{
    int apple, p;
    p = 10;
    apple = sizeof(int) * p;
    printf("sizeof(int) * p value is: %d\n", apple);
    printf("Answer:value depends on the type of p\n");
    printf("\tp is integer, * means multiplication\n");
    printf("\tp is pointer, will be compiled error\n");
    return 0;
}
