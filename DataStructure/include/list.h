#ifndef __LINKED_LIST_H_
#define __LINKED_LIST_H_

/*** start for sequence list ***/
/* typedef for sequence List */
#define SQLIST_MAXNUMS (1024)

/* NOTE: all data saved in sequence list will be typechanged to unsigned long */
typedef struct sqList_st {
    unsigned long *data;
    int length;
} sqList_t;
typedef sqList_t* sqList;

/*** function definition for sequence list ***/
extern int initSqListWithNums(unsigned long maxNums, sqList list);
extern int initSqList(sqList list);
extern void releaseSqList(sqList list);

/*** end for sequence list ***/

/*** start for sequence list ***/
/* typedef for linked List */
typedef struct linkedList_st {
    struct linkedList_st *next;
} linkedList_t;

typedef linkedList_t* linkedList;

/*** function definition for linked list ***/
extern int initLinkedList(linkedList list);
extern void releaseLinkedList(linkedList list);
/*** end for lined list ***/

#endif /* __LINKED_LIST_H_ */
