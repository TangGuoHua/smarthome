/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�������PIR̽ͷ
�̵���ģ��
��������

���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2013��10��01��  �Ƴ���  ��ʼ�汾
2014��01��27��  �Ƴ���  ����initDelay()

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "adc.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 91

sfr AUXR   = 0x8E;

sbit LIGHT_R = P1^4;  //�������� ��10K������
sbit PIR = P3^5;      //���͵���⴫����
sbit RELAY_HEATER = P3^7; //��ˮ���̵�����30A��
sbit RELAY_LIGHT1 = P1^7; //�ƿؼ̵���1
sbit RELAY_LIGHT2 = P1^6; //�ƿؼ̵���2


unsigned char light1Mode = 2; //0�����أ�1��������2���Զ�
unsigned char lightOnThreshold = 60; // ���Ƶ���ֵ

unsigned char heaterMode = 0; //0�����أ���0������

unsigned char timerCounter10ms = 0;
unsigned int timerSendData = 0;

// Flag for sending data to Pi
bit sendDataNow = 0;


//������ʱ 
//����NodeID������ԼΪ500*NodeID�������ʱ
//�����Ǳ������нڵ�ͬʱ�ϵ磬������5���Ӽ������������ɵ�ͨѶ��ײ
void initDelay(void)
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=167;c>0;c--)
	        for(b=171;b>0;b--)
	            for(a=16;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}




//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 91}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr);
}


//���ص�ǰ����ֵ
unsigned char getBrightness()
{
	return (255-getADCResult(4));
}



//��������ֵ����Ƭ��eeprom
void saveLightOnThreshold( unsigned char x )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, x );
}

//��Ƭ��eepromȡ�ÿ�����ֵ
unsigned char getLightOnThreshold()
{
	return eepromRead(0x0000);
}


void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	//unsigned char toAddr[3]= {97, 83, 175}; //
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID

	sendData[1] = PIR;
	sendData[2] = getBrightness(); //����
	sendData[3] = lightOnThreshold; 
		
	sendData[4] = ~RELAY_LIGHT1;
	sendData[5] = ~RELAY_LIGHT2;
	sendData[6] = ~RELAY_HEATER;

	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	
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
    TR0 = 1;
}

void initRelays()
{
	RELAY_LIGHT1=1;
	RELAY_LIGHT2=1;
	RELAY_HEATER=1;
}

void main()
{

	bit lastPIR=0;
	bit thisPIR=0;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//��ʼ���ж�0
	initINT0();
    
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��ADC
	initADC();
	
	//ȡ�ÿ�����ֵ
	lightOnThreshold = getLightOnThreshold();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ��timer0
	initTimer0();
	
	//��ʼ����ʱ
	initDelay();
	
	
	
	while(1)
	{
		//ȡ�ô������ĵ�ǰֵ
		thisPIR = PIR;

		//Send data to Pi
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();
		}
		
		// �ƿ�1
		if( light1Mode == 0 ) //����
		{
			RELAY_LIGHT1=1;
		}
		else if( light1Mode ==1 ) //����
		{
			RELAY_LIGHT1=0;
		}
		else if( light1Mode ==2 ) //�Զ�
		{
			if(RELAY_LIGHT1) //��ǰ�������
			{
				if( thisPIR && getBrightness()<=lightOnThreshold ) //���ˣ��һ�����������ֵ����
				{
					RELAY_LIGHT1 = 0; //����
				}
				
			}
			else //��ǰ��������
			{
				RELAY_LIGHT1 = ~thisPIR; //����������ص�
			}
		}
		
		//��¼��ǰֵ
		lastPIR = thisPIR;

	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char tmp;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): �̶�Ϊ1
	// *(receivedData+1): ���Ƶ�������ֵ
	// *(receivedData+2): �ƿؼ̵���1�Ĺ���ģʽ
	// *(receivedData+3): �ƿؼ̵���2�Ĺ���ģʽ
	// *(receivedData+4): ��ˮ���̵�������ģʽ
	
	//������ֵ
	tmp = *(receivedData+1);
	if( tmp!= lightOnThreshold )
	{
		//threshold changed, let's save it.
		lightOnThreshold = tmp;
		saveLightOnThreshold( lightOnThreshold );
	}
	
	light1Mode = *(receivedData+2);
	
	RELAY_HEATER = (*(receivedData+4))==0?1:0;
}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100��10ms����1��
	{
		timerCounter10ms=0;
		
		if( ++timerSendData == 300 ) //ÿ300����Pi����һ������
		{
			timerSendData = 0;
			sendDataNow = 1;
		}
		

	}
}

