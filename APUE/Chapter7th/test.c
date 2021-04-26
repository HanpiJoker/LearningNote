#include "apue.h"
#include <stdlib.h>

#define BUFFSIZE 10

int main(void)
{
    char* min;
    int i, j;

    for (i = 0; i < 2; i++) {
        min = malloc(sizeof(char) * BUFFSIZE);
        for (j = 0; j < BUFFSIZE; j++)
            min[j] = j;
        free(min);
    }
    return 0;
}
