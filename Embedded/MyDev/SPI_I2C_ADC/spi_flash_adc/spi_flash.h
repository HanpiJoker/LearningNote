#ifndef __SPI_FLASH_H_
#define __SPI_FLASH_H_

void SPI_Flash_ReadID (int *pMID, int *pDID);
void SPI_Flash_Init(void);
void SPI_Flash_Program(unsigned int address, unsigned char *buf, int len);
void SPI_Flash_Read(unsigned int address, unsigned char *buf, int len);
void SPI_Flash_Erase_Sector(unsigned int address);

#endif
