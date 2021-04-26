#include "apue.h"
#define BSZ 48
int main()
{
    FILE *fp1, *fp2, *fp3;
    char buf1[BSZ], buf2[BSZ], buf3[BSZ];
    //方案一
    memset(buf1, 'a', BSZ - 2);
    buf1[BSZ - 2] = '\0';
    buf1[BSZ - 1] = 'X';
    if ((fp1 = fmemopen(buf1, BSZ, "w+")) == NULL)
        err_sys("fmemopen failed");
    fprintf(fp1, "hello, world");
    //调用fflush函数引起缓冲区冲洗
    fflush(fp1);
    printf("1.After fflush: %s\n", buf1);
    printf("1.Len of string in buf = %ld\n", (long)strlen(buf1));

    memset(buf1, 'b', BSZ - 2);
    buf1[BSZ - 2] = '\0';
    buf1[BSZ - 1] = 'X';
    //二次输入数据为"nihao"，长度较短
    fprintf(fp1, "nihao");
    fclose(fp1);
    printf("1.After close: %s\n", buf1);
    printf("1.Len of string in buf = %ld\n\n", (long)strlen(buf1));

    //方案二
    memset(buf2, 'a', BSZ - 2);
    buf2[BSZ - 2] = '\0';
    buf2[BSZ - 1] = 'X';
    if ((fp2 = fmemopen(buf2, BSZ, "w+")) == NULL)
        err_sys("fmemopen failed");
    fprintf(fp2, "hello, world");
    //调用fseek函数引起缓冲区冲洗，偏移值设为首部
    fseek(fp2, 0, SEEK_SET);
    printf("2.After fseek: %s\n", buf2);
    printf("2.Len of string in buf = %ld\n", (long)strlen(buf2));

    memset(buf2, 'b', BSZ - 2);
    buf2[BSZ - 2] = '\0';
    buf2[BSZ - 1] = 'X';
    //二次输入数据为"nihao"，长度较短
    fprintf(fp2, "nihao");
    fclose(fp2);
    printf("2.After close: %s\n", buf2);
    printf("2.Len of string in buf = %ld\n\n", (long)strlen(buf2));

    //方案三
    memset(buf3, 'a', BSZ - 2);
    buf3[BSZ - 2] = '\0';
    buf3[BSZ - 1] = 'X';
    if ((fp3 = fmemopen(buf3, BSZ, "w+")) == NULL)
        err_sys("fmemopen failed");
    fprintf(fp3, "hello, world");
    //调用fseek函数引起缓冲区冲洗，偏移值设为首部
    fseek(fp3, 0, SEEK_SET);
    printf("3.After fseek: %s\n", buf3);
    printf("3.Len of string in buf = %ld\n", (long)strlen(buf3));

    memset(buf2, 'b', BSZ - 2);
    buf2[BSZ - 2] = '\0';
    buf2[BSZ - 1] = 'X';
    //二次输入数据为"hello, world! How are you?"，长度较长
    fprintf(fp3, "hello, world! How are you?");
    fclose(fp3);
    printf("3.After close: %s\n", buf3);
    printf("3.Len of string in buf = %ld\n\n", (long)strlen(buf3));

    return (0);
}
