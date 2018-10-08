#ifndef __UART_H
#define __UART_H

#include "p18cxxx.h"



#define UART_DISABLE_INTERRUPT()     	PIE1bits.RCIE=0;
#define UART_ENABLE_INTERRUPT()      	PIE1bits.RCIE=1;


#define pin_en1485 LATBbits.LATB1   //485收发使能引脚
#define RX_4851() pin_en1485 = 1
#define TX_4851() pin_en1485 = 0

void UART_Init();
void UartSendByte (unsigned char mydata_byte);
void UartSendBytes (unsigned char*buf,unsigned int nLen);

extern int dwTickCount,dwIntTick;
extern unsigned char sendBuf[16],receBuf[16];
extern unsigned char checkoutError;
extern unsigned char receTimeOut;

extern unsigned char sendCount;
extern unsigned char receCount;
extern unsigned char sendPosi;

#endif