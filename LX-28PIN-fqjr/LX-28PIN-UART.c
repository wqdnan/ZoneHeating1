//-------------------------------------------------------------------------------//
//                            LIXUE 电子工作室                                   //
//                       http://lixuediy.taobao.com                              //
//                                版权所有                                       //
//                       EMAIL:lixue51@126.com                                   //
//                       Mobile:13109884800    QQ:83637020                       //
//                       Develop: MPLAB IDE V8.92 + MCC18 V3.46                  //
//                       PIC MCU: PIC18F26K80                                    //
//                       File: LX-28PIN-UART.c                                   //
//                       DATE: 2016-04-21    Version:  5.0                       //
//-------------------------------------------------------------------------------//
//
// 配 置 位：菜单栏 Help->Topics-> PIC18 Config Settings 左侧选择对应芯片型号
//
// 头 文 件：C:\Program Files\Microchip\mplabc18\v3.46\h
//
// 实验描述：使用串口助手发送数据,数码管循环显示接收缓冲区数据
//
// 注意事项：外部8MHz  9600波特率 N 8 1 格式
//           
//-------------------------------------------------------------------------------

#include "p18cxxx.h"              //头文件
#include "delays.h"
//#include "SEG.h"
#include "\hardware\IIC.h"
#include "\hardware\ADC.h"
#include "\hardware\EEPROM.h"

//for modbus
#include "\modbus\TIM.h"
#include "\modbus\UART.h"
#include "\modbus\MODBUS.h"

//end for modbus


#define _DEBUG 1
#define _ID_WRITE 1
#define _ID_NUM 4


//#pragma config FOSC = HS1         //外部 8MHz
#pragma config WDTEN  = OFF       //关闭 WDT
#pragma config PLLCFG = OFF       //关闭 PLL
#pragma config XINST  = OFF       //关闭 Extended 
#pragma config SOSCSEL= DIG       //数字 I/O
/*
#define LED11  LATCbits.LATC4      //LED宏定义
#define LED22  LATCbits.LATC5
#define LED2  LATCbits.LATC0
#define LED3  LATAbits.LATA5
#define LED4  LATAbits.LATA3
#define LED5  LATAbits.LATA2

#define LED0_ON() LED11 = 0
#define LED0_OFF() LED11 = 1
#define LED1_ON() LED22 = 0
#define LED1_OFF() LED22 = 1
*/
unsigned char fixedTimeFlag  = 0;

void Interrupt_High(void);      //中断函数 

//-------------------------------------------------------------------------------
//	高优先级中断向量
//-------------------------------------------------------------------------------
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
	_asm
	goto      Interrupt_High     // 跳到中断程
	_endasm
}

//-------------------------------------------------------------------------------
//	高优先级中断服务程序
//-------------------------------------------------------------------------------
#pragma interrupt Interrupt_High
void Interrupt_High(void)
{
	static unsigned char temp;
	static unsigned int c10ms = 0,c1s = 0;
	if(PIR1bits.RCIF)            // 接收中断
	{
		PIR1bits.RCIF = 0;
		if(RCSTAbits.FERR||RCSTAbits.OERR){
                RCSTAbits.SPEN=0;                   //reboot EUSART clear FERR & OERR
                RCSTAbits.SPEN=1;
                PIR1bits.RCIF=0;
                LATCbits.LATC3=0;
            }
            else{
            receTimeOut = 5; //20ms内没有接受到数据超时 接收计数归0
            receBuf[receCount] = RCREG;
            receCount ++;
            receCount &= 0x0f;		// 最多只接收16个字节数据
           
        }
	}

	if(PIR1bits.TMR1IF)          //T1中断
	{
		PIR1bits.TMR1IF = 0;     //清除标志位
		TMR1H = TIM1H_CNT;            //初值
		TMR1L = TIM1L_CNT;
		//开始功能函数
		dwIntTick++;
        bt1ms = 1;
        c10ms++;
		c1s ++;
        if(c10ms >= 10)
        {
            c10ms = 0;      //10ms计时器清零
            bt10ms = 1;

        }
		if(c1s >= 1000)
		{
			c1s = 0;
			fixedTimeFlag = 0x35;
		}
		
	}
	
}
unsigned char EE_RD_Buffer[16] = {0x00};   //读缓冲

//-------------------------------------------------------------------------------
//	主程序
//-------------------------------------------------------------------------------
void main(void)
{
	unsigned char temp = 0;      //变量
	unsigned char cnt = 0;      //变量
	unsigned char str[3] = {0};
	float iic_data = 0;
	float tempF = 0;
	unsigned char i =0;
	TIM2_PWM_Init();             //TMR2 PWM 输出初始化
	TIM1_Init();                 //TMR1初始化 	
	
	UART_Init();                 //串口初始化
#ifndef _DEBUG
	I2C_Master_Init();	
#endif	

	INTCONbits.PEIE = 1;         //外设中断
	INTCONbits.GIE  = 1;         //系统中断
	T1CONbits.TMR1ON= 1;         //使能TMR1


	Delay10KTCYx(160);           //延时
/*
//感觉没有效果 重新下载后则失效了
#ifdef _ID_WRITE
	temp = (unsigned char)_ID_NUM;
	EE_writebyte(1,temp);
#else
	slaveNum = EE_readbyte(1);
#endif
*/	
	//--------------------------------------------------------------------------
	Delay10KTCYx(30);            //延时

#ifndef _DEBUG
	EE_Write_Byte(00,REG_IIC);
#endif

	while(1)
	{

		//modbus
		timerProc();
		checkComm0Modbus();
		//end modbus
		if(fctn16Flag == 0x35)//接收到modbus的写寄存器操作
		{
			fctn16Flag = 0;
			//tempF = (registerCtntRcv[0]&0xFF00)>>8;
			//tempF = tempF*10 + (registerCtntRcv[0]&0x00FF);
			//tempF = tempF*PWM_CYCLE/100;
			tempF = registerCtntRcv[0]&0xFF;
			tempF = 100-tempF;
			setDutyCycle_CCP2(tempF);
		}
#ifndef _DEBUG		
		if(fixedTimeFlag == 0x35)//定时采样中
		{
			fixedTimeFlag = 0;
			iic_data = EE_Read_Byte(00);
			tempF = ((iic_data/32768.0*2.048)*VOL_TO_TMPTURE_A+VOL_TO_TMPTURE_B)*10+4000;//扩大1000倍
			registerCtntSnd[0] = (unsigned short )tempF;
		
			//registerCtntSnd[0] =(tempF/1000%10)<<8;
			//registerCtntSnd[0] += (tempF/100%10);
			//registerCtntSnd[1] = (tempF/10%10)<<8;
			//registerCtntSnd[1] += (tempF%10);
		}
#endif
		
/*
		TX_4851();
		
		while(TXSTA1bits.TRMT==0);
		TXREG = (unsigned int)(tempF/10)%10+'0';
		while(TXSTA1bits.TRMT==0);
		TXREG = (unsigned int)(tempF)%10+'0';
		while(TXSTA1bits.TRMT==0);
		TXREG = '.';
		while(TXSTA1bits.TRMT==0);
		TXREG = (unsigned int)(tempF*10)%10+'0';
		while(TXSTA1bits.TRMT==0);
		TXREG = (unsigned int)(tempF*100)%10+'0';
		while(TXSTA1bits.TRMT==0);
		TXREG = (unsigned int)(tempF*1000)%10+'0';
		while(TXSTA1bits.TRMT==0);
		TXREG = (unsigned int)(tempF*10000)%10+'0';
		while(TXSTA1bits.TRMT==0);
		TXREG = 'V';
		while(TXSTA1bits.TRMT==0);
		TXREG = 0x0a;
		while(TXSTA1bits.TRMT==0);
		RX_4851();
*/
//-----------------------------------------------------
/*
		if(rxFlag == 0x35)
		{
			rxFlag = 0;
			txLen = 0;
			TX_4851();

			while(txLen < 10)
			{
				if(TXSTA1bits.TRMT)
				{
					TXREG = RXBuffer[txLen];  // 将收到的数据发出
					txLen ++;
				}
			}
			RX_4851();	
		}
*/
//----------------------------------------------------------
		//Delay10KTCYx(500);

	}
}

//-------------------------------------------------------------------------------
