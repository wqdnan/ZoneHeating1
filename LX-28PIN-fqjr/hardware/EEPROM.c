#include "EEPROM.h"


//-------------------------------------------------------------------------------
//	EEPROM读程序
//-------------------------------------------------------------------------------	
unsigned char EE_readbyte(unsigned char Addr)
{
	EEADR  = Addr;                //写入低地址          
	EECON1bits.EEPGD = 0;         //访问EEPROM
	EECON1bits.CFGS  = 0;
	EECON1bits.RD    = 1;
	return EEDATA;
}

//-------------------------------------------------------------------------------
//	EEPROM写程序
//-------------------------------------------------------------------------------	
void EE_writebyte(unsigned char Addr,unsigned char Byte)
{  
	ClrWdt();                     //喂狗
	EEADR  = Addr;                //写地址 
	EEDATA = Byte;                //写数据
	EECON1bits.EEPGD = 0;         //访问EEPROM
	EECON1bits.CFGS  = 0;
	EECON1bits.WREN = 1;          //允许写
	
	INTCONbits.GIE = 0;           //关中断
	EECON2 = 0x55;                //写入密钥
	EECON2 = 0xaa;
	EECON1bits.WR = 1;            //写操作

	INTCONbits.GIE  = 1;          //开中断
	EECON1bits.WREN = 0;          //禁止写
	while(EECON1bits.WR);         //写完成
}