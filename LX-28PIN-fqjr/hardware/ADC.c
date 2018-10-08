#include "ADC.h"


//-------------------------------------------------------------------------------
//	ADC��ʼ��
//-------------------------------------------------------------------------------
void ADC_Init(void)
{	
	ADCON0 = 0b00000101;          // ADCģ��ʹ��  AN1ͨ��ѡ��
	ANCON0 = 0b00000010;          // AN1 ADC���� ��������ΪIO
	ANCON1 = 0b00000000;          // ����IO
	ADCON2 = 0b10111111;          // �Ҷ���  FRC	
}

//-------------------------------------------------------------------------------
//	ADC�ɼ�����
//-------------------------------------------------------------------------------


float GET_ADValue(void) 
{
	unsigned int SampleADValue[8];    //8�β���
	unsigned int AvgADValue = 0;      //AD����ƽ��ֵ
	unsigned char ADConverStep = 0;   //AD����Ƶ��
	unsigned char i;
	unsigned int ConverValue;
	//for(i=0;i<8;i++)
    //{
		do
		{
		}while(ADCON0bits.GO_NOT_DONE);//�ȴ�ת�����
		ConverValue = ADRESH;          //����10λ���
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