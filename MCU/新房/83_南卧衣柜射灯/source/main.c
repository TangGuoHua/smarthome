/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
�̵���ģ��10A*2·


���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��10��18��  �Ƴ���  ��ʼ�汾


����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

//STC11F04E 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"

// Node ID
#define NODE_ID 83

sfr AUXR   = 0x8E;

sbit RELAY1 = P1^7;
sbit RELAY2 = P1^6; 

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char functionNum = 0;
volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerCounter1s = 0;

//��ʱ5�� STC11F04E 4MHz
void delay5s(void)
{
    unsigned char a,b,c;
    for(c=191;c>0;c--)
        for(b=189;b>0;b--)
            for(a=137;a>0;a--);
}


//������ʱ 
//�����Ǳ������нڵ�ͬʱ�ϵ磬��������ͬ��ʱ��������������ɵ�ͨѶ��ײ
void initDelay(void)
{
	//120��
    unsigned n;
    for(n=0;n<24;n++)
	{
		delay5s();
	}

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


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void initRelays()
{
	//��ʼ���̵���
	RELAY1 = 1;
	RELAY2 = 1;
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 83, 231, 83}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost()
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3�ֽڵ�ַ
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = functionNum;// Function Number, 1-regular status update, 51-respond to 50
	sendData[2] = ~RELAY1;
	sendData[3] = ~RELAY2;
	
	TR0 = 0;//Pause timer0
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);
	
	//24L01��ʼ��������
	startRecv();
	TR0 = 1;//Resume timer0
	
}


void main()
{
	unsigned char readAm2321Result = 0;
	
	//��ʼ���̵���
	initRelays();
	
	//��ʼ���ж�0
	initINT0();
	
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
		if(sendDataNow)
		{
			sendDataNow=0;
			
			//��������������
			sendDataToHost();	

		}
	}
}



//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	TR0 = 0;//Pause timer0

	
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 20: //ִ��������践��
			
			//�ֽ�1������1�ż̵�������ģʽ��0�����أ�1��������
			RELAY1 = (*(receivedData+2)==0)?1:0;
			
			//�ֽ�2������2�ż̵�������ģʽ��0�����أ�1��������
			RELAY2 = (*(receivedData+3)==0)?1:0;		
			break;
		

		
		case 50: //������������״̬
			functionNum = 51;//�ر�50
			timerCounter1s = 0;//��ʱ���¿�ʼ
			sendDataNow = 1;		
			break;
	
	}//end of switch
	
	
	TR0 = 1;//Resume timer0

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
			functionNum = 1;//��ʱ״̬����
			sendDataNow = 1;
		}
	}
}