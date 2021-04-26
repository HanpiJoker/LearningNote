#ifndef __S3C2440_SPI_H__
#define __S3C2440_SPI_H__

void SPI_Init(void);
void SPI_Send_Byte (unsigned char val);
unsigned char SPI_Rev_Byte(void);
#endif
