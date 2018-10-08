#include "TIM.h"

#include "UART.h"
unsigned int b1ms;
unsigned int bt1ms,b10ms,bt10ms,b100ms,bt100ms;

//定时一个1ms 的中断函数，用于modbus检测接收间隔
void TIM1_Init()
{
	TMR1H = TIM1H_CNT;                //8MHz  定一个1ms的定时中断
	TMR1L = TIM1L_CNT;        
	T1CONbits.T1CKPS = 1;        //预分频1:2
	T1CONbits.RD16 = 1;          //16位TIMR
	T1CONbits.TMR1CS = 0;        //FOSC/4
	PIE1bits.TMR1IE	= 1;         //TMR1中断
	
}

//定时处理 modbus
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
            if(receTimeOut==0 && receCount>0)   //判断通讯接收是否超时
            {
                receCount = 0;      //将接收地址偏移寄存器清零
				checkoutError = 0;
            }
        }
	}
	if(bt100ms)
	{
		bt100ms = 0;
		b100ms = 1;
	}
    if(bt10ms)      //判断中断10ms标志位是否1
    {
        bt10ms = 0;     //清中断10ms标志位
		b10ms = 1;
        c200ms++;                   //200ms计时器加1
        if(c200ms >= 20)            //判断是否计时到200ms
        {
            c200ms = 0;             //清200ms计时器
        }
    }
}

//-------------------------------------------------------------------------------
//	CCP2初始化 用于PWM输出 暂时设定频率周期为5KHz
//-------------------------------------------------------------------------------
void TIM2_PWM_Init(void)
{
	TRISCbits.TRISC2 = 0;       //设定RC2 输出低为发送 对应CCP2的PWM引脚
	CCP2CON = 0b00001100;         //CCP2 为PWM模式 
	CCPTMRS = 0b00000000;         //TMR2为时钟
	PIE3bits.CCP2IE = 0;          //CCP2中断禁止
	PIR3bits.CCP2IF = 0;          //CCP2 PWM模式不使用
	T2CON = 0b01001100;           //后分频比 1:10, T2ON 预分频比为1
	PR2 = TIM2_CYCLE;              //PWM周期 = (PR2+1)*4*TOSC*(TMR2预分频)
	                              //TOSC = 1/FOSC
	CCPR2L = 0x00;                //占空比 0
}
//--------------------------------------------------------
//修改CCP2 对应为 RC2 引脚的占空比 
//在预分频=1情况下，Duty=0 代表0%  Duty=100 代表100%
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
//使用设定温度和实际温度进行PID闭环控制，得出当前的PWM控制占空比来控制温度
//定时执行，执行周期设定为10ms的整数倍
//setTmpture-从modbus处获取到的两字节的内容
//crtTmpture-从IIC处获取到的两字节的内容
//------------------------------------------------------
void PID_Control(unsigned short setTmpture,unsigned short crtTmpture)
{
	static const float A = Kp_tpture*(1+T/Ti_tpture+Td_tpture);
	static const float B = Kp_tpture*(1+2*Td_tpture/T);
	static const float C = Kp_tpture*Td_tpture/T;
	static const PWM_H = 1000;
	static const PWM_L = -1000;
	static float err = 0,err_l = 0,err_ll = 0;//定义偏差，上次偏差，上上次偏差
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
	//需要限幅吧
	if(PWM > PWM_H)
		PWM = PWM_H;	
	else if(PWM < PWM_L)
		PWM = PWM_L;
	//调用PWM占空比驱动功率器件
	dutyCycle = 100-(PWM/(PWM_H+PWM_L)*100);
	setDutyCycle_CCP2((unsigned int)dutyCycle);
}