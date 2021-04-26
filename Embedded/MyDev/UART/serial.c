#include "serial.h"
#include "s3c24xx.h"

#define TXD0READY         (1<<2)
#define RXD0READY         (1)

#define PCLK              50000000         // init.c中的clock_init函数设置PLCK为50MHZ
#define UART_CLK          PCLK             // UART0的时钟源设为PCLK
#define UART_BAUD_RATE    115200           // 波特率
#define UART_BRD          ((UART_CLK / (UART_BAUD_RATE * 16)) - 1)

/*
 * 初始化UART0
 * 115200, 8N1, 无流控
 */
void uart0_init (void)
{
    GPHCON |= 0xa0; 
    GPHUP  = 0x0c;

    ULCON0 = 0x03;
    UCON0 = 0x05;
    UFCON0 = 0x00;
    UMCON0 = 0x00;
    UBRDIV0 = UART_BRD;
}

/*
 * 接受字符
 */

void putc (unsigned char c)
{
    while (!(UTRSTAT0 & TXD0READY));

    UTXH0 = c;
}

unsigned char getc (void)
{
    while (!(UTRSTAT0 & RXD0READY));

    return URXH0;
}

int isDigit (unsigned char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

int isLetter (unsigned char c)
{
    if (c >= 'a' && c <= 'z') {
        return 1;
    } 
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else
        return 0;
}
