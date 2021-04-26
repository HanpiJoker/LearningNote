#define GPFCON (*(volatile unsigned long *)0x56000050)
#define GPFDAT (*(volatile unsigned long *)0x56000054)

/* int main (void)
{
    GPFCON = 0x00001500;
    GPFDAT = 0x00000040;

    return 0;
}
 */

/* 位操作实现GPIO操作 */

#define GPF_OUT(i)  (1<<(i*2))

void delay (unsigned long xms){
    for (; xms>0; xms--);
}
int main (void)
{
    int i = 4;
    GPFCON = GPF_OUT(4) | GPF_OUT(5) | GPF_OUT(6);
    while (1)
    {
        GPFDAT = ~(1<<i);
        delay (100000);
        if (++i == 7)
            i = 4;
    }
}
