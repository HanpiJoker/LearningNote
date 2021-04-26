#ifndef __AT24C02_H_
#define __AT24C02_H_

void at24c02_write(unsigned char address, unsigned char data);
unsigned char at24c02_read(unsigned char address);

#endif
