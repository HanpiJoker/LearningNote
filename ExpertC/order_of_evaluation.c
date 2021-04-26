#include <stdio.h>
int i = 0;

int h(void)
{
    printf("h\n");
    return i += 2;
}

int f(void)
{
    printf("f\n");
    return i++;
}

int g(void)
{
    printf("g\n");
    return i += 3;
}

int main(void)
{
    int x = f() + g() * h();
    printf("value of f() + g() * h(): %d\n", x);
    return 0;
}
