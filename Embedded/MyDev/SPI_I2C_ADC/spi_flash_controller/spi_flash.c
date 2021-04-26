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

static void SPI_Flash_Write_Enable(int enable)
{
 	if (enable) {
		SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
		SPI_Send_Byte (0x06);
		SPI_Flash_Set_CS(1);
	} else {
		SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
		SPI_Send_Byte (0x04);
		SPI_Flash_Set_CS(1);
	}
}

static unsigned char SPI_Flash_Read_Status_Reg1(void)
{
	unsigned char val;
	SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
	SPI_Send_Byte (0x05);
	val = SPI_Rev_Byte();
	SPI_Flash_Set_CS(1);

	return val;
}

static unsigned char SPI_Flash_Read_Status_Reg2(void)
{
	unsigned char val;
	SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
	SPI_Send_Byte (0x35);
	val = SPI_Rev_Byte();
	SPI_Flash_Set_CS(1);

	return val;
}
static void SPI_Flash_Wait_Busy(void)
{
	/* BUSY  --- reg1 bit 0
	 * BUSY == 1    ---  busy
	 * BUSY == 0    ---  free
	 */
	 while(SPI_Flash_Read_Status_Reg1() & 1);
}

static void SPI_Flash_Write_Status_Reg(unsigned char reg1, unsigned char reg2)
{
	SPI_Flash_Write_Enable(1);

	SPI_Flash_Set_CS(0);	  // selection of SPI_Flash

	SPI_Send_Byte (0x01);
	SPI_Send_Byte (reg1);
	SPI_Send_Byte (reg2);

	SPI_Flash_Set_CS(1);
	
	SPI_Flash_Wait_Busy();
}

static void SPI_Flash_Clean_Protect_Regs(void)
{
	unsigned char reg1, reg2;
	
	reg1 = SPI_Flash_Read_Status_Reg1();
	reg2 = SPI_Flash_Read_Status_Reg2();

	reg1 &= ~(1 << 7);	
	reg2 &= ~(1 << 0);

	SPI_Flash_Write_Status_Reg(reg1, reg2);
}

static void SPI_Flash_Clean_Protect_Data(void)
{
	/* cmp = 0, b2 ~ b0 = 0b000;
	 * cmp  ---  reg2 : bit 6
	 * b2 ~ b0   ---- reg1 : bit4 ~ bit2
	 */ 
	unsigned char reg1, reg2;
	
	reg1 = SPI_Flash_Read_Status_Reg1();
	reg2 = SPI_Flash_Read_Status_Reg2();

	// reg1 &= ~((1 << 4)|(1 << 3)|(1 << 2));	
	reg1 &= ~(7 << 2);
	reg2 &= ~(1 << 6);
 	
	SPI_Flash_Write_Status_Reg(reg1, reg2);
}

// erase 4K
void SPI_Flash_Erase_Sector(unsigned int address)
{
	SPI_Flash_Write_Enable(1);

	SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
	SPI_Send_Byte (0x20);
	SPI_Flash_Send_Address (address);
	SPI_Flash_Set_CS(1);

	SPI_Flash_Wait_Busy();
}

// Program
void SPI_Flash_Program(unsigned int address, unsigned char *buf, int len)
{
	int i;
	
	SPI_Flash_Write_Enable(1);

	SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
	
	SPI_Send_Byte (0x02);
	SPI_Flash_Send_Address (address);
	
	for (i = 0; i < len; i++)
		SPI_Send_Byte (buf[i]);
	
	SPI_Flash_Set_CS(1);
	
	SPI_Flash_Wait_Busy();
}

void SPI_Flash_Read(unsigned int address, unsigned char *buf, int len)
{
	int i;
	
	//SPI_Flash_Write_Enable(1);

	SPI_Flash_Set_CS(0);	  // selection of SPI_Flash
	
	SPI_Send_Byte (0x03);
	SPI_Flash_Send_Address (address);
	
	for (i = 0; i < len; i++)
		buf[i] = SPI_Rev_Byte();
	
	SPI_Flash_Set_CS(1);
	
	//SPI_Flash_Wait_Busy();

}

void SPI_Flash_Init(void)
{
	SPI_Flash_Clean_Protect_Regs();
	SPI_Flash_Clean_Protect_Data();

}

