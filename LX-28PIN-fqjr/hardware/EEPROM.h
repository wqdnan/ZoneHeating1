#ifndef __EEPROM_H
#define __EEPROM_H

#include "p18cxxx.h"              //ͷ�ļ�

unsigned char EE_readbyte(unsigned char Addr);
void EE_writebyte(unsigned char Addr,unsigned char Byte);

#endif
