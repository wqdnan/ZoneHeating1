#include "ADC.h"


//-------------------------------------------------------------------------------
//	ADC初始化
//-------------------------------------------------------------------------------
void ADC_Init(void)
{	
	ADCON0 = 0b00000101;          // ADC模块使能  AN1通道选择
	ANCON0 = 0b00000010;          // AN1 ADC输入 其他设置为IO
	ANCON1 = 0b00000000;          // 数字IO
	ADCON2 = 0b10111111;          // 右对齐  FRC	
}

//-------------------------------------------------------------------------------
//	ADC采集程序
//-------------------------------------------------------------------------------


float GET_ADValue(void) 
{
	unsigned int SampleADValue[8];    //8次采样
	unsigned int AvgADValue = 0;      //AD采样平均值
	unsigned char ADConverStep = 0;   //AD采样频步
	unsigned char i;
	unsigned int ConverValue;
	//for(i=0;i<8;i++)
    //{
		do
		{
		}while(ADCON0bits.GO_NOT_DONE);//等待转换完成
		ConverValue = ADRESH;          //处理10位结果
		ConverValue = (ConverValue << 8) + ADRESL;
		SampleADValue[ADConverStep] = ConverValue;
		if (ADConverStep == 7)  
		{
			ConverValue = 0;
			for (i = 0; i < 8 ; i++)
			{
				ConverValue += SampleADValue[i];
			}
			AvgADValue = ConverValue >> 3;
			ADConverStep = 0;
		}
		else
		{
			ADConverStep++;
		}
   // }
	return SampleADValue[ADConverStep];
}