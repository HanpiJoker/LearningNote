/*************************************************************************
    > File Name: main.c
    > Author: SpaceYu
    > Mail: 15829341747@163.com 
    > Created Time: 2017年05月10日 星期三 13时44分03秒
 ************************************************************************/
#define GPFCON      (*(volatile unsigned long *)0x56000050)
#define GPFDAT      (*(volatile unsigned long *)0x56000054)
#define GPF4_OUT    (1 <<(4 * 2))
#define GPF5_OUT    (1 <<(5 * 2))
#define GPF6_OUT    (1 <<(6 * 2))

void wait(unsigned long dly)
{
    for (; dly > 0; dly--);
}
int main(int argc, const char *argv[])
{   
    //unsigned long i = 5;
    GPFCON = GPF4_OUT | GPF5_OUT| GPF6_OUT;
    while (1)
    {
        GPFDAT =  (~(5 << 4));
        wait(100000);
        GPFDAT =  (~(2 << 4));
        wait(100000);
        GPFDAT =  (~(0 << 4));
        wait(100000);
    }
    return 0;
}
