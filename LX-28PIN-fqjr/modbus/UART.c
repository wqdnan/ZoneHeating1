
#include "TYPE.h"
#include "UART.h"



int	dwTickCount,dwIntTick;//时钟
unsigned char sendBuf[16], receBuf[16];//发送接收缓冲区
unsigned char checkoutError;// ==2 偶校验错
unsigned char receTimeOut;//接收超时

unsigned char sendCount;
unsigned char receCount;

void UART_Init()
{
	TRISBbits.TRISB1 = 0;       //设定RB1 收发使能 输出低为发送 高位接收
	TRISCbits.TRISC7 = 1;       // 设置串口
	TRISCbits.TRISC6 = 0;
	
	SPBRG = 51;                 // 波特率 9600 N 8 1 
	RCSTAbits.SPEN = 1;         // 串口使能
	TXSTAbits.BRGH = 1;         // 高速模式
	TXSTAbits.SYNC = 0;         // 异步模式
	TXSTAbits.TXEN = 1;         // 发送允许
	RCSTAbits.CREN = 1;         // 允许接受
		
	PIE1bits.RCIE  = 1;         // 接收中断
	PIE1bits.TXIE  = 0;         // 发送中断

	RX_4851();
}

//*************************************************************************************
// Send one byte via UART
//*************************************************************************************
void UartSendByte(unsigned char mydata_byte) {

    TXREG = mydata_byte;       // transmit data
    while(!TXSTAbits.TRMT);    // make sure buffer full bit is high before transmitting

    
}

void UartSendBytes (unsigned char*buf,unsigned int nLen)
{
	UINT i;
	TX_4851();
	for(i=0;i<100;i++);//延时
	for (i = 0;i < nLen;i ++)
		UartSendByte (buf[i]);
	RX_4851();
}