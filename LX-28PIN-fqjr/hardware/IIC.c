#include "IIC.h"


/*
IIC ��ģʽ ͨ�� ʹ��RC3 ��SCL�� RC4 ��SDA�� 
*/
void I2C_Master_Init(void)  // I2C ģ���ʼ��
{
	
	TRISC |= 0b00011000;    //add �Ƚ�ʹ�õ���������Ϊ�������� RC3 RC4 (�ⲿ��Ҫ��������)
	SSPCON1 = 0b00101000;   // SSPEN = 1, Master Mode
	SSPCON2 = 0b00000000;   // GCEN = 0
	SSPADD  = 0b00001011;   // 100KHz @8MHz  ò�Ʋ��ԣ�100KZӦ�ö�ӦΪ19 ��00010011��
}

void IdleI2C(void)          // I2C ���߿��м��
{
	while ((SSPCON2&0x1F) || (SSPSTATbits.R_W))
	continue;
}
void StartI2C(void)         // I2C ��������
{
	SSPCON2bits.SEN = 1;
}
void RestartI2C(void)       // I2C ������������
{
	SSPCON2bits.RSEN = 1;
}
void StopI2C(void)          // I2C ����ֹͣ
{
	SSPCON2bits.PEN = 1;
}
void AckI2C(void)           // I2C ����Ӧ��
{
	SSPCON2bits.ACKDT = 0;
	SSPCON2bits.ACKEN = 1;
}
void NotAckI2C(void)        // I2C ������Ӧ��
{
	SSPCON2bits.ACKDT = 1;
	SSPCON2bits.ACKEN = 1;
}
void I2C_Done(void)         // I2C �����жϼ��
{
	while(!PIR1bits.SSPIF);
	PIR1bits.SSPIF = 0;
}
unsigned char ReadI2C(void) // I2C ���߶�����
{
	SSPCON2bits.RCEN = 1;
	while(!SSPSTATbits.BF);
	return SSPBUF;
}
unsigned char WriteI2C(unsigned char WR_DATA) // I2C ����д����
{
	SSPBUF = WR_DATA;                 // write 1 byte to SSPBUF
	if(SSPCON1bits.WCOL)  return 0;   // Write failure
	else                  return 1;   // Write successful
} 

//-------------------------------------------------------------------------------                                      
// ����˵���� EEPROM д1�ֽ�            �趨��ַΪ8λ                                                                                
//-------------------------------------------------------------------------------
void EE_Write_Byte( unsigned int addr,unsigned char data )
{
	unsigned char addrH,addrL;
	addrH = (unsigned char)((addr >>8)&0x1F);
	addrL = (unsigned char)(addr);
	IdleI2C();                      // ensure module is idle
  	StartI2C();                     // Start condition
	I2C_Done();                     // Wait Start completed				

	WriteI2C(WRITE_IIC);                 // Write Control+Write
	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
	I2C_Done();                     // Clear SSPIF flag

//	WriteI2C(addrH);                // Write Address to EEPROM
//	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
//	I2C_Done();
	
//	WriteI2C(addrL);                // Write Address to EEPROM
//	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
//	I2C_Done();

	WriteI2C(data);                 // Write Data to EEPROM
	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
	I2C_Done();

	StopI2C();                      // Stop condition
	I2C_Done();                     // Wait the Stop completed
}

//-------------------------------------------------------------------------------                                      
// ����˵���� EEPROM дn�ֽ�            �趨��ַΪ16λ                                                                                
//-------------------------------------------------------------------------------
void EE_SEQU_Write(unsigned int addr,unsigned char length,unsigned char *dptr)
{
	unsigned char addrH,addrL;
	addrH = (unsigned char)((addr >>8)&0x1F);
	addrL = (unsigned char)(addr);
	
	IdleI2C();             	        // ensure module is idle
  	StartI2C();                     // Start condition
	I2C_Done();                     // Wait Start condition completed
	
//	WriteI2C(CMD0);                 // Write Control+Write to EEPROM
//	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
//	I2C_Done();                     // Clear SSPIF flag

	WriteI2C(addrH);                // Write Address to EEPROM
	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
	I2C_Done();
	
	WriteI2C(addrL);                // Write Address to EEPROM
	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
	I2C_Done();

	while (length != 0)             // Check write completed ?
	{
		WriteI2C(*dptr);            // Write data to EEPROM
		while(SSPCON2bits.ACKSTAT); // wait Acknowledge from EEPROM
		I2C_Done();	
		dptr++;                     // Point to next byte
		length--;
	}
	
	StopI2C();                      // Stop condition
	I2C_Done();                     // Wait the Stop condition completed
}	

//-------------------------------------------------------------------------------                                      
// ����˵���� EEPROM ��1�ֽ�            �趨��ַΪ8λ                                                                                
//-------------------------------------------------------------------------------
unsigned int EE_Read_Byte(unsigned int addr)
{
	unsigned int f;	
	IdleI2C();                      // ensure module is idle
  	StartI2C();                     // Start condition
	I2C_Done();                     // Wait Start condition completed

	WriteI2C(READ_IIC);                 // Write Control to EEPROM    
	while(SSPCON2bits.ACKSTAT);     // test for ACK condition, if received
	I2C_Done();                     // Clear SSPIF flag

//	WriteI2C(addrH);                // Write Address to EEPROM
//	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
//	I2C_Done();
	
//	WriteI2C(addrL);                // Write Address to EEPROM
//	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
//	I2C_Done();

//  RestartI2C();                   // initiate Restart condition
//	I2C_Done();

//	WriteI2C(CMD1);                 // Write Control to EEPROM              
//	while(SSPCON2bits.ACKSTAT);     // test for ACK condition, if received
//	I2C_Done();                     // Clear SSPIF flag

	f=ReadI2C();                    // Enable I2C Receiver & wait BF=1 until received data
	I2C_Done();                     // Clear SSPIF flag

	f= f*256 + ReadI2C();                    // Enable I2C Receiver & wait BF=1 until received data
	I2C_Done();                     // Clear SSPIF flag

	NotAckI2C();                    // Genarate Non_Acknowledge to EEPROM
	I2C_Done();	   
                     
	StopI2C();                      // send STOP condition
	I2C_Done();                     // wait until stop condition is over 

	return(f);                      // Return Data from EEPROM 
}

//-------------------------------------------------------------------------------                                      
// ����˵���� EEPROM ��n�ֽ�           �趨��ַΪ16λ                                                                                
//-------------------------------------------------------------------------------
void EE_SEQU_Read(unsigned int addr,unsigned char length,unsigned char *dptr)
{
	unsigned char addrH,addrL;
	addrH = (unsigned char)((addr >>8)&0x1F);
	addrL = (unsigned char)(addr);
	
	IdleI2C();                      // ensure module is idle
  	StartI2C();                     // Start condition
	I2C_Done();                     // Wait Start condition completed

//	WriteI2C(CMD0);                 // Write Control to EEPROM    
//	while(SSPCON2bits.ACKSTAT);     // test for ACK condition, if received
//	I2C_Done();                     // Clear SSPIF flag

	WriteI2C(addrH);                // Write Address to EEPROM
	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
	I2C_Done();
	
	WriteI2C(addrL);                // Write Address to EEPROM
	while(SSPCON2bits.ACKSTAT);     // wait Acknowledge from EEPROM
	I2C_Done();                     // Clear SSPIF flag

   	RestartI2C();                   // initiate Restart condition
	I2C_Done();

//	WriteI2C(CMD1);                 // Write Control to EEPROM              
//	while(SSPCON2bits.ACKSTAT);     // Test for ACK condition, if received
//	I2C_Done();                     // Clear SSPIF flag

	while (length!=0)
	{
		*dptr=ReadI2C();            // Store EEPROM data to buffer 
		I2C_Done();	
		dptr++;	
		length--;
				
		if (length == 0) NotAckI2C();
		else             AckI2C();  // send a acknowledge to EEPROM
		
		I2C_Done();
	}

	StopI2C();                      // send STOP condition
	I2C_Done();                     // wait until stop condition is over 
}