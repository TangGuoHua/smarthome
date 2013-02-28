/*
STC 12C5608AD
4MHz�ⲿ����
*/

#include "reg52.h"
#include "nrf24L01Node.h"

sfr CCON = 0xD8;
sbit CR = CCON^6;
sfr CMOD = 0xD9;
sfr CL = 0xE9;
sfr CH = 0xF9;
sfr CCAPM0 = 0xDA;
sfr CCAP0L = 0xEA;
sfr CCAP0H = 0xFA;

sfr CCAPM2 = 0xDC;
sfr CCAP2L = 0xEC;
sfr CCAP2H = 0xFC;

//sbit LED=P3^7;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
sfr  AUXR       =   0x8e;       //�����Ĵ���            Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
                                //λ����                T0x12   T1x12   UM0x6   EADCI   ESPI    ELVDI   -       -
                                //��ʼֵ=0000,00xx      0       0       0       0       0       0       x       x
								
unsigned char x = 0;

void initPWM()
{
	CCON = 0;
	
	CL = 0;
	CH = 0;
	CMOD = 0x02;
	CCAP0H = CCAP0L = 0;
	CCAP2H = CCAP2L = 0;
	CCAPM0 = 0X42;
	CCAPM2 = 0X42;
	
	CR =1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void main()
{
	//��ʼ���ж�0
	initINT0();

	//��ʼ��24L01
	nrf24L01Init();
	
	//����24L01Ϊ����ģʽPRX
	nrfSetRxMode();
	
	initPWM();
	
	while(1)
	{
	}
}

//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	//unsigned char level[25]={0, 1, 2, 3, 4, 6, 9, 13, 16, 20, 25, 32, 38, 45, 55, 65, 80, 90, 110, 130, 150, 170, 190, 220, 255};
	unsigned char level[11]={0, 3, 8, 16, 30, 50, 75, 100, 140, 190, 255};

	receivedData = nrfGetReceivedData();
	if( *receivedData <= 10 )
	{
		CCAP0H = CCAP0L = level[*receivedData];
	}
	
	if( *++receivedData <= 10 )
	{
		CCAP2H = CCAP2L = level[*receivedData];
	}
}