#ifndef __IIC_H
#define __IIC_H

#include "p18cxxx.h"

/*
定义了IIC操作所需要的时序和寄存器操作，使用主模式
*/

#define WRITE_IIC    0x90             //写命令
#define READ_IIC     0x91             //读命令
#define REG_IIC      0x8C             //连续转换模式，16bit精度，PGA=1


void I2C_Master_Init(void);
void IdleI2C(void);
void StartI2C(void);
void RestartI2C(void);
void StopI2C(void);
void AckI2C(void);
void NotAckI2C(void);
void I2C_Done(void);
unsigned char ReadI2C(void);
unsigned char WriteI2C(unsigned char WR_DATA);
void EE_Write_Byte( unsigned int addr,unsigned char data );
void EE_SEQU_Write(unsigned int addr,unsigned char length,unsigned char *dptr);
unsigned int EE_Read_Byte(unsigned int addr);
void EE_SEQU_Read(unsigned int addr,unsigned char length,unsigned char *dptr);



#endif