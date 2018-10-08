//-------------------------------------------------------------------------------//
//                            LIXUE ���ӹ�����                                   //
//                       http://lixuediy.taobao.com                              //
//                                ��Ȩ����                                       //
//                       EMAIL:lixue51@126.com                                   //
//                       Mobile:13109884800    QQ:83637020                       //
//                       Develop: MPLAB IDE V8.92 + MCC18 V3.46                  //
//                       PIC MCU: PIC18F26K80                                    //
//                       File: LX-28PIN-UART.c                                   //
//                       DATE: 2016-04-21    Version:  5.0                       //
//-------------------------------------------------------------------------------//
//
// �� �� λ���˵��� Help->Topics-> PIC18 Config Settings ���ѡ���ӦоƬ�ͺ�
//
// ͷ �� ����C:\Program Files\Microchip\mplabc18\v3.46\h
//
// ʵ��������ʹ�ô������ַ�������,�����ѭ����ʾ���ջ���������
//
// ע������ⲿ8MHz  9600������ N 8 1 ��ʽ
//           
//-------------------------------------------------------------------------------

#include "p18cxxx.h"              //ͷ�ļ�
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


//#pragma config FOSC = HS1         //�ⲿ 8MHz
#pragma config WDTEN  = OFF       //�ر� WDT
#pragma config PLLCFG = OFF       //�ر� PLL
#pragma config XINST  = OFF       //�ر� Extended 
#pragma config SOSCSEL= DIG       //���� I/O
/*
#define LED11  LATCbits.LATC4      //LED�궨��
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

void Interrupt_High(void);      //�жϺ��� 

//-------------------------------------------------------------------------------
//	�����ȼ��ж�����
//-------------------------------------------------------------------------------
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
	_asm
	goto      Interrupt_High     // �����жϳ�
	_endasm
}

//-------------------------------------------------------------------------------
//	�����ȼ��жϷ������
//-------------------------------------------------------------------------------
#pragma interrupt Interrupt_High
void Interrupt_High(void)
{
	static unsigned char temp;
	static unsigned int c10ms = 0,c1s = 0;
	if(PIR1bits.RCIF)            // �����ж�
	{
		PIR1bits.RCIF = 0;
		if(RCSTAbits.FERR||RCSTAbits.OERR){
                RCSTAbits.SPEN=0;                   //reboot EUSART clear FERR & OERR
                RCSTAbits.SPEN=1;
                PIR1bits.RCIF=0;
                LATCbits.LATC3=0;
            }
            else{
            receTimeOut = 5; //20ms��û�н��ܵ����ݳ�ʱ ���ռ�����0
            receBuf[receCount] = RCREG;
            receCount ++;
            receCount &= 0x0f;		// ���ֻ����16���ֽ�����
           
        }
	}

	if(PIR1bits.TMR1IF)          //T1�ж�
	{
		PIR1bits.TMR1IF = 0;     //�����־λ
		TMR1H = TIM1H_CNT;            //��ֵ
		TMR1L = TIM1L_CNT;
		//��ʼ���ܺ���
		dwIntTick++;
        bt1ms = 1;
        c10ms++;
		c1s ++;
        if(c10ms >= 10)
        {
            c10ms = 0;      //10ms��ʱ������
            bt10ms = 1;

        }
		if(c1s >= 1000)
		{
			c1s = 0;
			fixedTimeFlag = 0x35;
		}
		
	}
	
}
unsigned char EE_RD_Buffer[16] = {0x00};   //������

//-------------------------------------------------------------------------------
//	������
//-------------------------------------------------------------------------------
void main(void)
{
	unsigned char temp = 0;      //����
	unsigned char cnt = 0;      //����
	unsigned char str[3] = {0};
	float iic_data = 0;
	float tempF = 0;
	unsigned char i =0;
	TIM2_PWM_Init();             //TMR2 PWM �����ʼ��
	TIM1_Init();                 //TMR1��ʼ�� 	
	
	UART_Init();                 //���ڳ�ʼ��
#ifndef _DEBUG
	I2C_Master_Init();	
#endif	

	INTCONbits.PEIE = 1;         //�����ж�
	INTCONbits.GIE  = 1;         //ϵͳ�ж�
	T1CONbits.TMR1ON= 1;         //ʹ��TMR1


	Delay10KTCYx(160);           //��ʱ
/*
//�о�û��Ч�� �������غ���ʧЧ��
#ifdef _ID_WRITE
	temp = (unsigned char)_ID_NUM;
	EE_writebyte(1,temp);
#else
	slaveNum = EE_readbyte(1);
#endif
*/	
	//--------------------------------------------------------------------------
	Delay10KTCYx(30);            //��ʱ

#ifndef _DEBUG
	EE_Write_Byte(00,REG_IIC);
#endif

	while(1)
	{

		//modbus
		timerProc();
		checkComm0Modbus();
		//end modbus
		if(fctn16Flag == 0x35)//���յ�modbus��д�Ĵ�������
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
		if(fixedTimeFlag == 0x35)//��ʱ������
		{
			fixedTimeFlag = 0;
			iic_data = EE_Read_Byte(00);
			tempF = ((iic_data/32768.0*2.048)*VOL_TO_TMPTURE_A+VOL_TO_TMPTURE_B)*10+4000;//����1000��
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
					TXREG = RXBuffer[txLen];  // ���յ������ݷ���
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
