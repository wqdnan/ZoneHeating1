#ifndef __IIC_H
#define __IIC_H

#include "p18cxxx.h"

/*
������IIC��������Ҫ��ʱ��ͼĴ���������ʹ����ģʽ
*/

#define WRITE_IIC    0x90             //д����
#define READ_IIC     0x91             //������
#define REG_IIC      0x8C             //����ת��ģʽ��16bit���ȣ�PGA=1


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