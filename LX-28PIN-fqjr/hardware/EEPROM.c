#include "EEPROM.h"


//-------------------------------------------------------------------------------
//	EEPROM������
//-------------------------------------------------------------------------------	
unsigned char EE_readbyte(unsigned char Addr)
{
	EEADR  = Addr;                //д��͵�ַ          
	EECON1bits.EEPGD = 0;         //����EEPROM
	EECON1bits.CFGS  = 0;
	EECON1bits.RD    = 1;
	return EEDATA;
}

//-------------------------------------------------------------------------------
//	EEPROMд����
//-------------------------------------------------------------------------------	
void EE_writebyte(unsigned char Addr,unsigned char Byte)
{  
	ClrWdt();                     //ι��
	EEADR  = Addr;                //д��ַ 
	EEDATA = Byte;                //д����
	EECON1bits.EEPGD = 0;         //����EEPROM
	EECON1bits.CFGS  = 0;
	EECON1bits.WREN = 1;          //����д
	
	INTCONbits.GIE = 0;           //���ж�
	EECON2 = 0x55;                //д����Կ
	EECON2 = 0xaa;
	EECON1bits.WR = 1;            //д����

	INTCONbits.GIE  = 1;          //���ж�
	EECON1bits.WREN = 0;          //��ֹд
	while(EECON1bits.WR);         //д���
}