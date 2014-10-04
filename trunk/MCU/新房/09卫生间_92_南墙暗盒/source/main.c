/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
DS18B20�¶ȴ�����
30A�̵���ģ��

���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��02��05��  �Ƴ���  ��ʼ�汾
2014��10��03��  �Ƴ���  ����С�����Զ�����ģʽ
                        ����EEPROM��������
						��Pi�������ݵ���ʱ��ԭ10s��ʱ������Ϊtimer0�жϼ���


����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

//STC11F04E 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"
#include "ds18B20.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 92

sfr AUXR   = 0x8E;

sbit RELAY_WATERHEATER = P3^7;  //С������ˮ�����Ƽ̵���

volatile unsigned char waterHeaterMode = 0; //0���أ�1������2���Զ����ܵ���PIR���ƣ�

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerCounter1s = 0;

//С������ˮ��ʱ
volatile bit waterHeaterDelayOn = 0; //С������ˮ��ʱ��ʼ��ֹͣ
volatile int waterHeaterDelayCounter1s = 0; 
volatile unsigned char waterHeaterDelayMinute = 0; //С������ˮ��ʱ������

//������ʱ 
//����NodeID������ԼΪ(500*NodeID)�������ʱ
//�����Ǳ������нڵ�ͬʱ�ϵ磬������5���Ӽ������������ɵ�ͨѶ��ײ
void initDelay(void)
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=167;c>0;c--)
	        for(b=172;b>0;b--)
	            for(a=16;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


// ��ʼ��Timer0
// ÿ10ms�����ж�1һ��
// ��STC 1T @4MHz �ⲿ����
void initTimer0(void)
{
	AUXR = AUXR|0x80;  //����T0Ϊ1Tģʽ
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}


//��ʼ���̵���
void initRelays()
{
	//ȫ��
	RELAY_WATERHEATER=1;
}


//��С������ˮ��ʱ����������Ƭ��eeprom
void saveWaterHeaterDelayMinute( unsigned char val )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, val );
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 92, 231, 92}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	float temperature;
	int intTemperature;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update
	
	// DS18B20ȡ���¶�ֵ
	temperature = readTemperature(1);
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
	sendData[2]= temperature<0 ? 1 : 0; //������������λ��1
	sendData[3]= intTemperature/10; //�¶ȵ���������
	sendData[4]= intTemperature%10; //�¶ȵ�С������
	
	sendData[5]= waterHeaterMode;
	sendData[6]= waterHeaterDelayMinute;
	sendData[7] = (~RELAY_WATERHEATER); //Water heater relay status
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96Ƶ����5�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv(); 
}



void main()
{
	//��ʼ���ж�0
	initINT0();
	
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��DS18B20
	initDS18B20();
	
	//ȡ��С������ˮ��ʱ������
	waterHeaterDelayMinute = eepromRead(0x0000);
	
	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
	
	// ��ʼ��Timer0
	initTimer0();
			
	while(1)
	{
		//��������������
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();	
		}
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
	
	if( *receivedData == 1 )//����Pi(#1)
	{
		//С��������ģʽ
		tmp = *(receivedData+1);
		
		//С��������
		if( tmp == 0 )//����
		{
			RELAY_WATERHEATER = 1;
			
			//���֮ǰС�������Զ�ģʽ����������ʱ��ˮ�����У�
			//��ֹͣ��ˮ��ʱ���������������
			waterHeaterDelayOn = 0; 
		}
		else if( tmp == 1 )//����
		{
			RELAY_WATERHEATER = 0;
			
			//���֮ǰС�������Զ�ģʽ����������ʱ��ˮ�����У�
			//��ֹͣ��ˮ��ʱ���������������
			waterHeaterDelayOn = 0; 
		}
		else if( tmp == 2 )//�Զ�
		{
			//����֮ǰС�����Ƿ�����ˮ�������ں���ģʽ��
			//��ֹͣ��ˮ��ֹͣ��ʱ����
			//�����Զ�ģʽ2��ϵͳ�����Ե�ǰ�Ƿ����˵�״̬
			//ֻ�еȴ���һ�������������˱�����ʱ���ſ�ʼ�Զ���ˮ
			RELAY_WATERHEATER = 1;
			waterHeaterDelayOn = 0; 
		}
		waterHeaterMode = tmp;
		
		
		//��ˮ��ʱ������	
		tmp = *(receivedData+2);
		if( tmp != waterHeaterDelayMinute )//�������仯��
		{
			waterHeaterDelayMinute=tmp;
			saveWaterHeaterDelayMinute( tmp );
		}
	}
	else if( *receivedData == 91 ) //���Ե���������(#92)
	{
		//С�����������Զ�ģʽ��
		//��������Զ�ģʽ����������Ե���������PIR������
		if( waterHeaterMode == 2 ) 
		{
			tmp = *(receivedData+1);
			if( tmp == 1)
			{
				//�����ˣ���ˮ
								
				waterHeaterDelayOn = 0; //��ˮ��ʱ��ʱֹͣ
				waterHeaterDelayCounter1s = 0;
				
				RELAY_WATERHEATER = 0; //��ʼ��ˮ
			}
			else if( tmp == 0 )
			{
				//�����ˣ�������ˮ(waterHeaterDelayMinute)���ӣ�Ȼ���ٹ�С����
				waterHeaterDelayCounter1s = 0;
				waterHeaterDelayOn = 1; //��ˮ��ʱ��ʼ��ʱ
			}
		}
	}
}



//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100��10ms����1��
	{
		timerCounter10ms=0;
		
		//��Pi��������
		if( ++timerCounter1s == 600 ) //ÿ600��һ��
		{
			timerCounter1s = 0;
			sendDataNow = 1;
		}
		
		//С������ʱ
		if( waterHeaterDelayOn )
		{
			if( ++waterHeaterDelayCounter1s >= waterHeaterDelayMinute*60 )
			{
				//��ʱ�ѵ�
				waterHeaterDelayOn = 0; //ֹͣ��ʱ��ʱ
				waterHeaterDelayCounter1s = 0; //����������
				RELAY_WATERHEATER = 1; //��С����
			}
		}
	}
}