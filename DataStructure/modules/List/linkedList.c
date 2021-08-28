#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "list.h"

int initLinkedList(linkedList list)
{
    list->next = NULL;
    return 0;
}

void releaseLinkedList(linkedList list)
{
    list->next = NULL;
}
