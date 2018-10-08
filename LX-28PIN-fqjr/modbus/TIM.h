#ifndef __TIM_H
#define __TIM_H

#include "p18cxxx.h"


#define TIM1H_CNT 0xFC//0xD8//    //用于设定10ms的定时周期   
#define TIM1L_CNT 0x18//0xF0//    //公式为：中断周期 =（65536-<TIM1H_CNT:TIM1L_CNT>）* 1us

#define TIM2_CYCLE 454          //公式为：PWM周期 = (TIM2_CYCLE+1)*4/FOSC*(TMR2预分频)  TMR2预分频=1
                                //TIM2_CYCLE=399 为14KHz   占空比设定的满幅调的使用512
                                //TIM2_CYCLE=454 为10KHz   
#define PWM_CYCLE 850           //(TIM2_CYCLE+1)*4
							    //公式为：占空比时间 = (CCPR2L:CCP2CON<5:4>)/FOSC*(TMR2预分频)  TMR2预分频=1


//for PID control start
#define VOL_TO_TMPTURE_A 491.4   //定义了IIC采样的电压值对应到温度的参数（PT100）
#define VOL_TO_TMPTURE_B -303.3  // 温度值 = A * 电压值 + B

#define Kp_tpture 1        //PID 温度闭环算法中的P I D参数
#define Ti_tpture 0.01
#define Td_tpture 0
#define T 0.01             //算法定时执行间隔

//for PID control end
extern unsigned int b1ms;
extern unsigned int bt1ms,b10ms,bt10ms,b100ms,bt100ms;

void TIM1_Init();
void timerProc(void);

void TIM2_PWM_Init(void);
void setDutyCycle_CCP2(unsigned int Duty);         //PWM 占空比转换
void PID_Control(unsigned short setTmpture,unsigned short crtTmpture);

#endif