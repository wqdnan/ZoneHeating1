#ifndef __TIM_H
#define __TIM_H

#include "p18cxxx.h"


#define TIM1H_CNT 0xFC//0xD8//    //�����趨10ms�Ķ�ʱ����   
#define TIM1L_CNT 0x18//0xF0//    //��ʽΪ���ж����� =��65536-<TIM1H_CNT:TIM1L_CNT>��* 1us

#define TIM2_CYCLE 454          //��ʽΪ��PWM���� = (TIM2_CYCLE+1)*4/FOSC*(TMR2Ԥ��Ƶ)  TMR2Ԥ��Ƶ=1
                                //TIM2_CYCLE=399 Ϊ14KHz   ռ�ձ��趨����������ʹ��512
                                //TIM2_CYCLE=454 Ϊ10KHz   
#define PWM_CYCLE 850           //(TIM2_CYCLE+1)*4
							    //��ʽΪ��ռ�ձ�ʱ�� = (CCPR2L:CCP2CON<5:4>)/FOSC*(TMR2Ԥ��Ƶ)  TMR2Ԥ��Ƶ=1


//for PID control start
#define VOL_TO_TMPTURE_A 491.4   //������IIC�����ĵ�ѹֵ��Ӧ���¶ȵĲ�����PT100��
#define VOL_TO_TMPTURE_B -303.3  // �¶�ֵ = A * ��ѹֵ + B

#define Kp_tpture 1        //PID �¶ȱջ��㷨�е�P I D����
#define Ti_tpture 0.01
#define Td_tpture 0
#define T 0.01             //�㷨��ʱִ�м��

//for PID control end
extern unsigned int b1ms;
extern unsigned int bt1ms,b10ms,bt10ms,b100ms,bt100ms;

void TIM1_Init();
void timerProc(void);

void TIM2_PWM_Init(void);
void setDutyCycle_CCP2(unsigned int Duty);         //PWM ռ�ձ�ת��
void PID_Control(unsigned short setTmpture,unsigned short crtTmpture);

#endif