#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "at24xx.h"
#include "s3c2440_spi.h"
#include "oled.h"
#include "spi_flash.h"
#include "adc_ts.h"

int main()
{
    char c;
    char str[200];
    int i;
    int address, data;
    unsigned int mid, did;
	
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)

	SPI_Init();
	OLED_Init();
	OLED_Print(0, 0, "SpaceYu HiWorld");

	SPI_Flash_ReadID(&mid, &did);
	printf ("SPI_Flash: MID = 0x%02x, DID = 0x%02x\n", mid, did);
	
	sprintf (str, "SPI : %02x, %02x", mid, did);
	
	OLED_Print(2, 0, str);
	
	SPI_Flash_Init();

	SPI_Flash_Erase_Sector (4096);
	SPI_Flash_Program (4096, "SpaceYu", 8);
	SPI_Flash_Read (4096, str, 8);
	printf ("SPI_Flash read from 4096: %s \n\r", str);
	OLED_Print(4, 0, str);
	
	i2c_init();
	at24c02_write(0, 0x60);
	OLED_Clear_Page (2);
	data = at24c02_read(0);
	if (data == 0x55) 
		OLED_Print(2, 0, "I2C OK");
	else
		OLED_Print(2, 0, "I2C FAILED");

	Test_Adc();

   
    return 0;
}
