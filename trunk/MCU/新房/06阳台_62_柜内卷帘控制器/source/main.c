/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�̵���ģ��


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��07��30��  �Ƴ���  ��ʼ�汾


����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
//#include "adc.h"
//#include "stcEEPROM.h"
//#include "ds18B20.h"

// Node ID
#define NODE_ID 62

sfr AUXR   = 0x8E;

//sbit LDR = P1^7;  //�������� ��10K������


sbit RELAY_POWER = P2^1; //�̵���1�����Ƶ�Դ
sbit RELAY_DIRECTION = P2^0; //�̵���2�����Ʒ��򣬳���-����������-�½�

sbit K1=P1^7; //��
sbit K2=P1^6; //��

unsigned char curtainMode = 1; //1:�ֶ���2���Զ�
unsigned char curtainOpenPercent;
bit motorDirection = 0; //����˶�����1������������������0���½����ؾ�����
bit motorOn = 0; //��￪��, 1������ 0����

unsigned char timerCounter10ms = 0;
unsigned char timerCounter1s = 0;
//unsigned int timerSendData = 0;

// Flag for sending data to Pi
//bit sendDataNow = 0;


void delay200ms()
{
    unsigned char a,b,c;
    for(c=29;c>0;c--)
        for(b=70;b>0;b--)
            for(a=97;a>0;a--);
}

void stopMotor()
{
	//ͣ����Դ
	RELAY_POWER = 1;
	
	TR0=0;
	
	delay200ms();
	
	//����ѡ��̵����ϵ�
	RELAY_DIRECTION = 1;
}

void startMotor()
{
	//�趨����
	RELAY_DIRECTION = motorDirection;
	
	delay200ms();
	
	//ͨ��
	RELAY_POWER = 0;
	
	TR0 = 1;
}

////������ʱ 
////����NodeID������ԼΪ500*NodeID�������ʱ
////�����Ǳ������нڵ�ͬʱ�ϵ磬������5���Ӽ������������ɵ�ͨѶ��ײ
//void initDelay(void)
//{
//	//4MHz Crystal, 1T STC11F04E
//    unsigned char a,b,c,d;
//    for(d=NODE_ID;d>0;d--)
//	    for(c=167;c>0;c--)
//	        for(b=171;b>0;b--)
//	            for(a=16;a>0;a--);
//}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}




//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 62}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr); //����92Ƶ����3�ֽڵ�ַ
}
//
//
////���ص�ǰ����ֵ
//unsigned char getBrightness()
//{
//	return (255-getADCResult(7));
//}
//
//
//
////��������ֵ����Ƭ��eeprom
//void saveLightOnThreshold( unsigned char threshold1, unsigned char threshold2 )
//{
//	eepromEraseSector( 0x0000 );
//	eepromWrite( 0x0000, threshold1 );
//	eepromWrite( 0x0002, threshold2 );
//}
//
//////��Ƭ��eepromȡ�ÿ�����ֵ
////unsigned char getLightOnThreshold()
////{
////	return eepromRead(0x0000);
////}
//
//
void sendDataToHost( unsigned char a, unsigned char b, unsigned char c, unsigned char d )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	unsigned char tmp;

		
	sendData[0] = NODE_ID;//Node ID

	sendData[1] = a;
	sendData[2] = b;
	sendData[3] = c; 
	sendData[4] = d; 


	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//��Pi����, 96Ƶ����3�ֽڵ�ַ��16�ֽ�����
	
	//24L01��ʼ��������
	startRecv(); 
}


// ��ʼ��Timer0
void initTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    //TR0 = 1;
}

void initRelays()
{
	RELAY_POWER=1;
	RELAY_DIRECTION=1;
}

void main()
{


	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//��ʼ���ж�0
	initINT0();
    
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��DS18B20
	//initDS18B20();
	
	//��ʼ��ADC
	//initADC();
	
	//ȡ�ÿ�����ֵ
	//light1OnThreshold = eepromRead(0x0000);
	//light2OnThreshold = eepromRead(0x0002);

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	//initDelay();
	
	//��ʼ��timer0
	initTimer0();
	
	
	
	while(1)
	{
//		if( motorOn && (RELAY_POWER==1) ) //˵�������ǰ���ڹ���
//		{
//			startMotor();
//		}

		if( K1==0 && RELAY_POWER == 1 )
		{
			timerCounter10ms = 0;
			timerCounter1s = 0;
			motorDirection=1;
			//TR0 = 1;
			startMotor();
			
			sendDataToHost( 1, 100, 0, 0 );
			
			delay200ms();
			delay200ms();
			
		}
		else if( K1==0 && RELAY_POWER == 0 )
		{
			//TR0 = 0;
			stopMotor();
			
			sendDataToHost( 1, 200, timerCounter1s, timerCounter10ms );
			delay200ms();
			delay200ms();
		}

		if( K2==0 && RELAY_POWER == 1 )
		{
			timerCounter10ms = 0;
			timerCounter1s = 0;
			motorDirection=0;
			//TR0 = 1;
			startMotor();
			
			sendDataToHost( 0, 100, 0, 0 );
			
			delay200ms();
			delay200ms();
			
		}
		else if( K2==0 && RELAY_POWER == 0 )
		{
			//TR0 = 0;
			stopMotor();
			
			sendDataToHost( 0, 200, timerCounter1s, timerCounter10ms );
			delay200ms();
			delay200ms();
		}
	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char rMode, rOpenPercent;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): ������NodeID
	// *(receivedData+1): ���ܺ�
	// *(receivedData+2): ��������ģʽ
	// *(receivedData+3): ����

	
	//������ֵ
	rMode = *(receivedData+0);
	rOpenPercent = *(receivedData+1);
	
	if( rMode == 0)
	{
		stopMotor();
	}
	else if( rMode == 1) //��
	{
		motorDirection=1;
		startMotor();
	}
	else if( rMode ==2 ) //��
	{
		motorDirection=0;
		startMotor();
	}
	
	
//	if( rOpenPercent != curtainOpenPercent )
//	{
//		if( rOpenPercent > curtainOpenPercent )
//		{
//			//������
//			motorDirection = 1;
//			motorOn = 1;
//		}
//		else
//		{
//			//�ش���
//			motorDirection = 0;
//			motorOn = 1;
//		}
//	}
	

	

}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100��10ms����1��
	{
		timerCounter10ms=0;
		timerCounter1s++;
		

		

	}
}
