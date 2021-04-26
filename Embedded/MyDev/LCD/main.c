/*************************************************************************
    > File Name: main.c
    > Author: SpaceYu
    > Mail: 15829341747@163.com 
    > Created Time: 2017年05月24日 星期三 18时03分15秒
 ************************************************************************/

#include <stdio.h>
#include "s3c24xx.h"
#include "serial.h"
#include "lcdlib.h"

int main()
{
    char c;
        
    uart0_init();

    while (1)
    {
        printf("\r\n#### Test My TFT LCD ####\r\n");
        printf("[1] TFT240320 8Bit\n\r");
        printf("[2] TFT240320 16Bit\n\r");
        printf("[3] TFT480272 8Bit\n\r");
        printf("[4] TFT480272 16Bit\n\r");
        printf("Enter your selection:");

        c = getc();
        printf("%c\n\r", c);
        switch (c) 
        {
            case '1':
            {
                Test_Lcd_Tft_8Bit_240320();
                break;
            }    
    
            case '2':
            {
                Test_Lcd_Tft_16Bit_240320();
                break;
            }
    
            case '3':
            {
                Test_Lcd_Tft_8Bit_480272();
                break;
            }
            case '4':
            {
                Test_Lcd_Tft_16Bit_480272();
                break;
            }
            
            default:
                break;
        }
    }
    return 0;
}
