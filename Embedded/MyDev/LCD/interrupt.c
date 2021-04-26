/*************************************************************************
    > File Name: interrupt.c
    > Author: SpaceYu
    > Mail: 15829341747@163.com 
    > Created Time: 2017年05月24日 星期三 17时51分01秒
 ************************************************************************/

#include "s3c24xx.h"

void (*isr_handle_array[50])(void);

void Dummy_isr(void)
{
    while(1);
}

void init_irq (void)
{
    int i = 0;
    for (i = 0; i < sizeof(isr_handle_array)/sizeof(isr_handle_array[0]); i++) 
    {
        isr_handle_array[i] = Dummy_isr;    
    }

    INTMOD = 0x0;
    INTMSK = BIT_ALLMSK;
}

void IRQ_Handle(void)
{
    unsigned long oft = INTOFFSET;

    if (oft == 4)
        EINTPEND = 1 << 7;
    SRCPND = 1 << oft;
    INTPND = INTPND;

    isr_handle_array[oft]();
}
