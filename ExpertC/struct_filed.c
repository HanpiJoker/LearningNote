#include <stdio.h>
/* 经试验, 在我的机器上的gcc 和clang 的结构体的成员对齐方式都是自然对齐,
 * 即结构体的size 一定可以被结构体中最大的成员的size 整除。对齐的原因是
 * 为了确保cpu访问时候的地址是对齐的, 避免带来不必要的效率降低。如果地址
 * 对齐, 那么当访问int型数据的其实地址是0x3, 就需要先访问0x0拿到第一个数据
 * 再访问0x4拿到后续的数据。目前在我的64位的机器上看到的是编译器采用的是依据
 * 结构体中的最大成员进行对齐，回头可以看看32位的机器的结果*/
struct pid_tag {
    unsigned int inactive :1;
    unsigned int          :1;
    unsigned int refcount :6; /* 1 + 1 + 6 = 8bit == 1Byte */
    unsigned int          :0; /* pad to next word boundary, 1Byte will pad to 4Byte */
    short pid_id; /* struct 4Byte align, so result is 8Byte */
    struct pid_tag *link;
};

struct pid_tag_no_bit {
    unsigned int inactive;   /* 4Byte */
    unsigned int padding;    
    unsigned int refcount;
    unsigned int pad;
    short pid_id;            /* 2Byte */
    struct pid_tag_no_bit *link;  /* 8Byte */
};

struct pid_tag_no_next {
    unsigned int inactive;   /* 4Byte */
    unsigned int padding;    
    unsigned int refcount;
    unsigned int pad;
    short pid_id;            /* 2Byte */
};

struct test_align {
    char param1;
    int param2;
    double param3;
};

int main(void)
{
    struct pid_tag_no_bit nobit;
    struct pid_tag tg;
    printf("nobit: %ld tag:%ld\n", sizeof(nobit), sizeof(tg)); 
    printf("point nobit: %ld tag:%ld\n", sizeof(struct pid_tag_no_bit *), sizeof(struct pid_tag *)); 
    printf("nonext: %ld\n", sizeof(struct pid_tag_no_next)); 
    printf("short size:%ld\n", sizeof(short));
    printf("unsigned int size:%ld\n", sizeof(unsigned int));
    printf("sizeof double:%ld\n", sizeof(double));
    printf("sizeof long long:%ld\n", sizeof(long long));
    printf("sizeof test_align:%ld\n", sizeof(struct test_align));
    return 0;
}
