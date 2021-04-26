#include"spi_flash.h"
#include"gpio_spi.h"
#include "s3c24xx.h"

static void SPI_Flash_Set_CS(char val)
{
	if (val) 
		GPGDAT |= (1<<2);
	else
		GPGDAT &= ~(1<<2);
}

static void SPI_Flash_Send_Address(unsigned int address)
{
	SPI_Send_Byte(address >> 16);
	SPI_Send_Byte(address >> 8);
	SPI_Send_Byte(address & 0xff);

}

void SPI_Flash_ReadID (int *pMID, int *pDID)
{
	SPI_Flash_Set_CS(0);      // selection of SPI_Flash
	
	SPI_Send_Byte (0x90);
	SPI_Flash_Send_Address (0);

	*pMID = SPI_Rev_Byte ();
	*pDID = SPI_Rev_Byte ();
	
	SPI_Flash_Set_CS(1);
}

