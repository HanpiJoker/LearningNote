#include <stdio.h>
int array[] = {23, 34, 12, 17, 204, 99, 16};
#define TOTAL_ELEMENTS (sizeof(array) / sizeof(array[0]))
int main(void)
{
    int d = -1, x = 0;
    if (d <= TOTAL_ELEMENTS - 2) {
        x = array[d + 1];
        printf("x = %d\n, test a bug", x);
    } else {
        printf("BUG_ON(): TOTAL_ELEMENTS return type is unsigned int\n");
        printf("\ttype of variable d is int. So type of d is promoted\n");
        printf("\tto unsigned int. Thus the if statement did not evaluate\n");
        printf("\tto true!!!");
    }

    return 0;
}
