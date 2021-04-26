#include "oled.h"
#include "s3c24xx.h"
#include "gpio_spi.h"
#include "oledfont.h"

static void OLED_Set_DC(unsigned char val)
{
	if (val) 
		GPGDAT |= (1<<4);
	else
		GPGDAT &= ~(1<<4);
}

static void OLED_Set_CS(unsigned char val)
{
	if (val) 
		GPFDAT |= (1<<1);
	else
		GPFDAT &= ~(1<<1);
}



static void OLED_Write_Cmd(unsigned char cmd)
{
	OLED_Set_DC(0);   // Send Command
	OLED_Set_CS(0);   // Chip Selection the OLED

	SPI_Send_Byte(cmd);


	OLED_Set_CS(1);   // Un-Selection the OLED
	OLED_Set_DC(1);   // Send
}

static void OLED_Write_Data(unsigned char data)
{
	OLED_Set_DC(1);   // Send data
	OLED_Set_CS(0);   // Chip Selection the OLED

	SPI_Send_Byte(data);

	OLED_Set_CS(1);   // Un-Selection the OLED
	OLED_Set_DC(1);   // Send
}

static void OLED_Set_Page_Addr_Mode(void)
{
	OLED_Write_Cmd(0x20);
	OLED_Write_Cmd(0x02);
}

static void OLED_Clear(void)
{
	int page, col, i;
	for (page = 0; page < 8; page++)
	{
		OLED_Set_Pos (page, 0);
		for (i = 0; i < 128; i++)
			OLED_Write_Data (0);
	}
}

void OLED_Init(void)
{
	/* Send Command to Oled, Let it init */
	OLED_Write_Cmd (0xAE); /*display off*/
	OLED_Write_Cmd (0x00); /*set lower column address*/
	OLED_Write_Cmd (0x10); /*set higher column address*/
	OLED_Write_Cmd (0x40); /*set display start line*/
	OLED_Write_Cmd (0xB0); /*set page address*/
	OLED_Write_Cmd (0x81); /*contract control*/
	OLED_Write_Cmd (0x66); /*128*/
	OLED_Write_Cmd (0xA1); /*set segment remap*/
	OLED_Write_Cmd (0xA6); /*normal / reverse*/
	OLED_Write_Cmd (0xA8); /*multiplex ratio*/
	OLED_Write_Cmd (0x3F); /*duty = 1/64*/
	OLED_Write_Cmd (0xC8); /*Com scan direction*/
	OLED_Write_Cmd (0xD3); /*set display offset*/
	OLED_Write_Cmd (0x00);
	OLED_Write_Cmd (0xD5); /*set osc division*/
	OLED_Write_Cmd (0x80);
	OLED_Write_Cmd (0xD9); /*set pre-charge period*/
	OLED_Write_Cmd (0x1f);
	OLED_Write_Cmd (0xDA); /*set COM pins*/
	OLED_Write_Cmd (0x12);
	OLED_Write_Cmd (0xdb); /*set vcomh*/
	OLED_Write_Cmd (0x30);
	OLED_Write_Cmd (0x8d); /*set charge pump enable*/
	OLED_Write_Cmd (0x14);

	OLED_Set_Page_Addr_Mode();
	OLED_Clear();	

	OLED_Write_Cmd (0xAF); /*display ON*/


}


void OLED_Set_Pos(int page, int col)
{
	OLED_Write_Cmd (0xB0 + page);   // Set Page Start Address for Page Addressing Mode

	OLED_Write_Cmd (col & 0xf);     // Set Lower Column Start Address for Page Addressing Mode
	OLED_Write_Cmd (0x10 + (col >> 4));   // Set Higher Column Start Address for Page Addressing Mode
}

void OLED_Put_char(int page, int col, char c)
{
	int i;
	
	// Get Matrix
	const unsigned char *dots = oled_asc2_8x16[c - ' '];
	
	// Send to OLED
	OLED_Set_Pos(page, col);
	for (i = 0; i < 8; i++)
		OLED_Write_Data(dots[i]);
	
	OLED_Set_Pos(page + 1, col);
	for (i = 0; i < 8; i++)
		OLED_Write_Data(dots[i + 8]);
}
/*
 * page   ---  0 ~ 7
 * col    ---  0 ~ 127
 * str    ---  8 x 16 pixels
 */
void OLED_Print(int page, int col, char *str)
{
	int i = 0;
	while (str[i])
	{
		OLED_Put_char(page, col, str[i]);
		col += 8;
		if (col > 127) {
			col = 0;
			page += 2;
		}
		i++;
	}
}
