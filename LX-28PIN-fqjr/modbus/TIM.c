#include "TIM.h"

#include "UART.h"
unsigned int b1ms;
unsigned int bt1ms,b10ms,bt10ms,b100ms,bt100ms;

//��ʱһ��1ms ���жϺ���������modbus�����ռ��
void TIM1_Init()
{
	TMR1H = TIM1H_CNT;                //8MHz  ��һ��1ms�Ķ�ʱ�ж�
	TMR1L = TIM1L_CNT;        
	T1CONbits.T1CKPS = 1;        //Ԥ��Ƶ1:2
	T1CONbits.RD16 = 1;          //16λTIMR
	T1CONbits.TMR1CS = 0;        //FOSC/4
	PIE1bits.TMR1IE	= 1;         //TMR1�ж�
	
}

//��ʱ���� modbus
void timerProc(void)
{
	static unsigned int c200ms;

	b1ms = 0;
	b10ms = 0;
	b100ms = 0;

//	INTCONbits.TMR0IE=0;
	dwTickCount = dwIntTick;
//	INTCONbits.TMR0IE=1;

	if(bt1ms)
	{
		bt1ms = 0;
		b1ms = 1;

        if(receTimeOut>0)
        {
            receTimeOut--;
            if(receTimeOut==0 && receCount>0)   //�ж�ͨѶ�����Ƿ�ʱ
            {
                receCount = 0;      //�����յ�ַƫ�ƼĴ�������
				checkoutError = 0;
            }
        }
	}
	if(bt100ms)
	{
		bt100ms = 0;
		b100ms = 1;
	}
    if(bt10ms)      //�ж��ж�10ms��־λ�Ƿ�1
    {
        bt10ms = 0;     //���ж�10ms��־λ
		b10ms = 1;
        c200ms++;                   //200ms��ʱ����1
        if(c200ms >= 20)            //�ж��Ƿ��ʱ��200ms
        {
            c200ms = 0;             //��200ms��ʱ��
        }
    }
}

//-------------------------------------------------------------------------------
//	CCP2��ʼ�� ����PWM��� ��ʱ�趨Ƶ������Ϊ5KHz
//-------------------------------------------------------------------------------
void TIM2_PWM_Init(void)
{
	TRISCbits.TRISC2 = 0;       //�趨RC2 �����Ϊ���� ��ӦCCP2��PWM����
	CCP2CON = 0b00001100;         //CCP2 ΪPWMģʽ 
	CCPTMRS = 0b00000000;         //TMR2Ϊʱ��
	PIE3bits.CCP2IE = 0;          //CCP2�жϽ�ֹ
	PIR3bits.CCP2IF = 0;          //CCP2 PWMģʽ��ʹ��
	T2CON = 0b01001100;           //���Ƶ�� 1:10, T2ON Ԥ��Ƶ��Ϊ1
	PR2 = TIM2_CYCLE;              //PWM���� = (PR2+1)*4*TOSC*(TMR2Ԥ��Ƶ)
	                              //TOSC = 1/FOSC
	CCPR2L = 0x00;                //ռ�ձ� 0
}
//--------------------------------------------------------
//�޸�CCP2 ��ӦΪ RC2 ���ŵ�ռ�ձ� 
//��Ԥ��Ƶ=1����£�Duty=0 ����0%  Duty=100 ����100%
//---------------------------------------------------------
void setDutyCycle_CCP2(unsigned int Duty)
{
	float cycle = 0;
	cycle = Duty*0.01;
	cycle = cycle*PWM_CYCLE;
	CCPR2L = (unsigned char)((unsigned int)cycle>>2);
	CCP2CON |= (unsigned char)(((unsigned int)cycle&0x0003)<<4);
}

//------------------------------------------------------
//ʹ���趨�¶Ⱥ�ʵ���¶Ƚ���PID�ջ����ƣ��ó���ǰ��PWM����ռ�ձ��������¶�
//��ʱִ�У�ִ�������趨Ϊ10ms��������
//setTmpture-��modbus����ȡ�������ֽڵ�����
//crtTmpture-��IIC����ȡ�������ֽڵ�����
//------------------------------------------------------
void PID_Control(unsigned short setTmpture,unsigned short crtTmpture)
{
	static const float A = Kp_tpture*(1+T/Ti_tpture+Td_tpture);
	static const float B = Kp_tpture*(1+2*Td_tpture/T);
	static const float C = Kp_tpture*Td_tpture/T;
	static const PWM_H = 1000;
	static const PWM_L = -1000;
	static float err = 0,err_l = 0,err_ll = 0;//����ƫ��ϴ�ƫ����ϴ�ƫ��
	static float PWM_tmp = 0;
	static float PWM = 0;
	float setTmp = 0,crtTmp = 0;
	float dutyCycle = 0;
	//start to count
	crtTmp = (crtTmpture/32768.0*2.048)*VOL_TO_TMPTURE_A+VOL_TO_TMPTURE_B;
	setTmp = setTmpture * 0.01;
	err_ll = err_l;
	err_l = err;
	err = setTmp - crtTmp;
	PWM_tmp = (A*err-B*err_l+C*err_ll);
	PWM += PWM_tmp;
	//��Ҫ�޷���
	if(PWM > PWM_H)
		PWM = PWM_H;	
	else if(PWM < PWM_L)
		PWM = PWM_L;
	//����PWMռ�ձ�������������
	dutyCycle = 100-(PWM/(PWM_H+PWM_L)*100);
	setDutyCycle_CCP2((unsigned int)dutyCycle);
}