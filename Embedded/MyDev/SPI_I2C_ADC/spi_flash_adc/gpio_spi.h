#ifndef __GPIO_SPI_H__
#define __GPIO_SPI_H__

void SPI_Init(void);
void SPI_Send_Byte (unsigned char val);
unsigned char SPI_Rev_Byte(void);
#endif

