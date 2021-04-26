#include "gpio_spi.h"
#include "s3c24xx.h"

/* Use GPIO simulate SPI 
 * GPG5    ---   SPIMISO (Input)
 * GPG6    ---   SPIMOSI (Output)
 * GPG7    ---   SPICLK  (Output)
 * GPG2    ---   FLASH_CSn    (Output)
 * GPG4    ---   OLED_DC      (Output)
 * GPF1    ---   OLED_CSn     (Output)
 */
static void SPI_GPIO_Init(void)
{
	GPFCON &= ~(3<<1*2);
	GPFCON |= (1<<1*2);
	GPFDAT |= (1<<1);      // de-selection 
	
	GPGCON &= ~((3<<2*2)|(3<<4*2)|(3<<5*2)|(3<<6*2)|(3<<7*2));
	GPGCON |= ((1<<2*2)|(1<<4*2)|(1<<6*2)|(1<<7*2));
	GPGDAT |= (1<<2);     // de-selection
}

void Spi_Init (void)
{
	// ³õÊ¼»¯Òý½Å
	SPI_GPIO_Init();
	
}

static void SPI_Set_CLK(unsigned char val)
{
	if (val)
		GPGDAT |= (1<<7);
	else
		GPGDAT &= ~(1<<7);
}

static void SPI_Set_DO(unsigned char val)
{
	if (val)
		GPGDAT |= (1<<6);
	else
		GPGDAT &= ~(1<<6);
}

void SPI_Send_Byte (unsigned char val)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		SPI_Set_CLK(0);
		SPI_Set_DO(val & 0x80);
		SPI_Set_CLK(1);
		val <<= 1;
	}
}