#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "at24xx.h"


int main()
{
    char c;
    char str[200];
    int i;
    int address, data;
    
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)
    
    i2c_init();
    
    while (1)
    {
        printf("\r\n##### AT24C02 ####\r\n");
        printf("[R] Read AT24C02\n\r");
        printf("[W] Write AT24C02\n\r");
        printf("Enter your selection: ");

        c = getc();
        printf("%c\n\r", c);
        switch (c)
        {
            case 'r':
            case 'R':
            {
                printf("Enter address: ");
                i = 0;
                do
                {
                    c = getc();
                    str[i++] = c;
                    putc(c);
                } while(c != '\n' && c != '\r');
                str[i] = '\0';

                while(--i >= 0)
                {
                    if (str[i] < '0' || str[i] > '9')
                        str[i] = ' ';
                }

                sscanf(str, "%d", &address);
				printf("\r\nread address = %d\r\n", address);
				data = at24c02_read(address);
				printf("data = %d\r\n", data);
                break;
            }
            
            case 'w':
            case 'W':
            {	
				printf("Enter address & data: ");
				i = 0;
				do
				{
					c = getc();
					str[i++] = c;
                    putc(c);
				} while(c != '\n' && c != '\r');
				str[i] = '\0';
				
				while(--i >= 0)
				{
					if (str[i] < '0' || str[i] > '9')
						str[i] = ' ';
				}
				sscanf(str, "%d %d", &address, &data);
				printf("write address %d with data %d\r\n", address, data);
                at24c02_write(address, data);
                break;
            }
        }
        
    }
    
    return 0;
}
