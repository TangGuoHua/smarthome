/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������

8λ�̵���ģ�� 

��Ϊ���������Ӷ˿ڲ�����ֻ��7�����ÿڣ�
2�ż̵���������������Ҫһֱͨ�磬��2�ż̵���������������ӣ�ʹ�����ĳ��մ���


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��06��29��  �Ƴ���  ��ʼ�汾
2014��10��04��  �Ƴ���  ����nrf24L01+����
                        �޸�2�ż̵���Ϊ���ܿ�
						������Pi��ʱ״̬����

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

//STC11F04E 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"


sfr AUXR   = 0x8E;

// Node ID
#define NODE_ID 51

sbit RELAY_1 = P1^3;
//sbit RELAY_2 = P1^3;
sbit RELAY_3 = P3^5;
sbit RELAY_4 = P1^5;
sbit RELAY_5 = P1^4;
sbit RELAY_6 = P1^7;
sbit RELAY_7 = P1^6;
sbit RELAY_8 = P3^7;


volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerSendData = 0;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;

//������ʱ 
//����NodeID������ԼΪ(NodeID*2)�����ʱ
//�����Ǳ������нڵ�ͬʱ�ϵ磬������5���Ӽ������������ɵ�ͨѶ��ײ
void initDelay(void)
{
	//4MHz Crystal, 1T
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=252;c>0;c--)
	        for(b=230;b>0;b--)
	            for(a=33;a>0;a--);
}

//��ʼ���̵���
void initRelays()
{
	RELAY_1=1;
	//RELAY_2=1;
	RELAY_3=1;
	RELAY_4=1;
	RELAY_5=1;
	RELAY_6=1;
	RELAY_7=1;
	RELAY_8=1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


// ��ʼ��Timer0
void initTimer0(void)
{
	AUXR = AUXR|0x80;  // T0, 1T Mode
	TMOD = 0x01;
	TH0 = 0x63;
	TL0 = 0xC0;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 51, 231, 51}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//�������ݸ�Pi
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update

	sendData[2] = ~RELAY_1;
	sendData[3] = 0; //RELAY_2���ã�����
	sendData[4] = ~RELAY_3;
	sendData[5] = ~RELAY_4;
	sendData[6] = ~RELAY_5;
	sendData[7] = ~RELAY_6;
	sendData[8] = ~RELAY_7;
	sendData[9] = ~RELAY_8;
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96Ƶ����5�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv(); 
}


void main()
{

	//��ʼ���̵���
	initRelays();
	
	//��ʼ��INT0 
	initINT0();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
	
	//��ʼ��timer0
	initTimer0();
	
	
	while(1)
	{
		//Send data to Pi
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
	unsigned char fromNodeID;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	fromNodeID = *(receivedData++); //�����ߵ�ID
	
	RELAY_1 = (*(receivedData++)==0)?1:0;
	receivedData++; // Relay_2 ���ã�����
	RELAY_3 = (*(receivedData++)==0)?1:0;
	RELAY_4 = (*(receivedData++)==0)?1:0;
	RELAY_5 = (*(receivedData++)==0)?1:0;
	RELAY_6 = (*(receivedData++)==0)?1:0;
	RELAY_7 = (*(receivedData++)==0)?1:0;
	RELAY_8 = (*(receivedData++)==0)?1:0;

}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100��10ms����1��
	{
		timerCounter10ms=0;
		
		if( ++timerSendData == 600 ) //ÿ600����Pi����һ������
		{
			timerSendData = 0;
			sendDataNow = 1;
		}
		

	}
}