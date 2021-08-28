#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "list.h"

int initSqListWithNums(unsigned long maxNums, sqList list)
{
    list->data = (unsigned long *)calloc(1, sizeof(unsigned long) * maxNums);
    if (!list->data)
        return errno;

    list->length = 0;
    return 0;
}

int initSqList(sqList list)
{
    return initSqListWithNums(SQLIST_MAXNUMS, list);
}

void releaseSqList(sqList list)
{
    if (list->data) {
        free(list->data);
        list->data = NULL;
    }

    list->length = 0;
}
