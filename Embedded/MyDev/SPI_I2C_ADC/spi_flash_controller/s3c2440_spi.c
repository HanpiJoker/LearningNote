#include "s3c2440_spi.h"
#include "s3c24xx.h"
#include <stdio.h>
/* Use GPIO simulate SPI 
 * GPG5    ---   SPIMISO (11b)
 * GPG6    ---   SPIMOSI (11b)
 * GPG7    ---   SPICLK  (11b)
 * GPG2    ---   FLASH_CSn    (Output)
 * GPG4    ---   OLED_DC      (Output)
 * GPF1    ---   OLED_CSn     (Output)
 */
static void SPI_GPIO_Init(void)
{
	GPFCON &= ~(3<<(1*2));
	GPFCON |= (1<<(1*2));
	GPFDAT |= (1<<1);      // de-selection 
	
	GPGCON &= ~((3<<(2*2))|(3<<(4*2))|(3<<(5*2))|(3<<(6*2))|(3<<(7*2)));
	GPGCON |= ((1<<(2*2))|(1<<(4*2))|(3<<(5*2))|(3<<(6*2))|(3<<(7*2)));
	GPGDAT |= (1<<2);     // de-selection
}

static void SPI_Controller_Init(void)
{
	/* OLED  100ns   10MHz
	 * Flash         104MHz
	 * Baud Rate 10MHz
	 * 10 = 50 / 2 / (value + 1)
	 * value = 1.5 = 2
	 * Baud Rate = 50 / 2 / 3 = 8.3MHz
	 */
	SPPRE0 = 2;
	SPPRE1 = 2;

	/* [6:5] -- 00  -- polling mode
	 * [4]   --  1  -- enable SCK
	 * [3]   --  1  -- master
	 * [2]   --  0  -- active high
	 * [1]   --  0  -- format A
	 * [0]   --  0  -- normal mode
	 */
	SPCON0 = (1<<3)|(1<<4);
	SPCON1 = (1<<3)|(1<<4);
}

void SPI_Init (void)
{
	// ³õÊ¼»¯Òý½Å
	SPI_GPIO_Init();
	SPI_Controller_Init();
}

void SPI_Send_Byte (unsigned char val)
{
	
	while(!(SPSTA1 & 1));
	SPTDAT1 = val;
}

unsigned char SPI_Rev_Byte(void)
{

	SPTDAT1 = 0xff;
	while(!(SPSTA1 & 1));
	return SPRDAT1;
}

