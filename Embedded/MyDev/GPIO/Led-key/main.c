/* EINT0 -- GPF0
 * EINT2 -- GPF2 
 * EINT11 -- GPG3
 * EINT19 -- GPG11
 * GPGCON -- 0x56000060
 * GPGDAT -- 0x56000064
 */

/*
 * EINT0 -- D10 -- LED1 -- GPF4
 * EINT2 -- D11 -- LED2 -- GPF5
 * EINT11 -- D12 -- LED4 -- GPF6
 * EINT19 --- ALL
*/
#define GPFCON (*(volatile unsigned long *)0x56000050)
#define GPFDAT (*(volatile unsigned long *)0x56000054)

#define GPGCON (*(volatile unsigned long *)0x56000060)
#define GPGDAT (*(volatile unsigned long *)0x56000064)

#define GPF_MSK(i)  (3<<(i*2))
#define GPG_MSK(i)  (3<<(i*2))
#define GPF_OUT(i)  (1<<(i*2))
#define GPF_IN(i)   (0<<(i*2))
#define GPG_IN(i)   (0<<(i*2))
#define KEY_ON(i)  (1<<i)
#define LED_ON(i)  (1<<i)

int main (void)
{
    unsigned long dwDat;

    GPFCON &= ~(GPF_MSK(4)|GPF_MSK(5)|GPF_MSK(6));
    GPFCON |= GPF_OUT(4) | GPF_OUT(5) | GPF_OUT(6);

    GPFCON &= ~(GPF_MSK(0)|GPF_MSK(2));
    GPFCON |= GPF_IN(0) | GPF_IN(2);

    GPGCON &= ~(GPG_MSK(3)|GPG_MSK(11));
    GPGCON |= GPG_IN(3) | GPG_IN(11);

    while (1)
    {
        dwDat = GPFDAT;
        if (dwDat & KEY_ON(0))
            GPFDAT |= LED_ON(4);
        else
            GPFDAT &= ~LED_ON(4);
        if (dwDat & KEY_ON(2))
            GPFDAT |= LED_ON(5);
        else
            GPFDAT &= ~LED_ON(5);
        
        dwDat = GPGDAT;
        if (dwDat & KEY_ON(3))
            GPFDAT |= LED_ON(6);
        else
            GPFDAT &= ~LED_ON(6);
        if (dwDat & KEY_ON(11))
            GPFDAT |= LED_ON(4)|LED_ON(5)|LED_ON(6);
        else
            GPFDAT &= ~(LED_ON(4)|LED_ON(5)|LED_ON(6));
    }
    return 0;
}
