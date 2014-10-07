/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
����AM2321��ʪ�ȴ�����

���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��01��26��  �Ƴ���  ��ʼ�汾
2014��01��27��  �Ƴ���  ����initDelay()
2014��02��11��  �Ƴ���  ����AM2321����ȫ���bug
2014��10��07��  �Ƴ���  ����NRF24L01+����

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

//STC11F04E 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"
#include "am2321.h"

// Node ID
#define NODE_ID 54

sfr AUXR   = 0x8E;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerCounter1s = 0;

//
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




//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( unsigned char readAm2321Result )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3�ֽڵ�ַ
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = readAm2321Result;
	sendData[2] = getAM2321Data(0);
	sendData[3] = getAM2321Data(1);
	sendData[4] = getAM2321Data(2);
	sendData[5] = getAM2321Data(3);
	sendData[6] = getAM2321Data(4);
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);
	
	//24L01��ʼ��������
	//startRecv(); 
}



void main()
{
	unsigned char readAm2321Result = 0;
	
	//��ʼ���ж�0
	initINT0();
	
	//init AM2321
	initAM2321();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	//startRecv(); 
	
	//��ʼ����ʱ
	//initDelay();
	
	// ��ʼ��Timer0
	initTimer0();	
			
	while(1)
	{
		if(sendDataNow)
		{
			sendDataNow=0;
			//����AM3231����������ȡ��һ�β�������ʪ��ֵ
			readAm2321Result = readAM2321( 2 );

			//��������������
			sendDataToHost( readAm2321Result );	

		}
	}
}



////�ⲿ�ж�0�������
////���ڴ�������24L01���ж�
//void interrupt24L01(void) interrupt 0
//{
//	unsigned char * receivedData;
//	
//	//��ȡ���յ�������
//	receivedData = nrfGetReceivedData();
//	
//	//�ֽ�1������1�ż̵�������ģʽ��0�����أ�1��������2����ʱ�أ�
//	relay1Mode = *(receivedData++);
//	if( relay1Mode == 0 ) RELAY_1=1;
//	else if( relay1Mode == 1 ) RELAY_1=0;
//	else if( relay1Mode == 2 )
//	{
//		relay1DelayOffTimerCount = 0; //��ʱ�ؼ�������
//	}
//
//	
//	//�ֽ�2������2�ż̵����Ĺ���ģʽ ��0�����أ�1��������2���¿أ�
//	relay2Mode = *(receivedData++);
//	if( relay2Mode == 0 ) RELAY_2=1;
//	else if( relay2Mode == 1 ) RELAY_2=0;
//	
//	//�ֽ�3���¶����� 10����
//	tUpper = *(receivedData++);
//	
//	//�ֽ�4���¶����� 10����
//	tLower = *(receivedData++);
//}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100��10ms����1��
	{
		timerCounter10ms=0;
		
		//��Pi��������
		if( ++timerCounter1s == 10 ) //ÿ600��һ��
		{
			timerCounter1s = 0;
			sendDataNow = 1;
		}
	}
}