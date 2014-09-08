/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
����DHT11��ʪ�ȴ�����


���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��09��04��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/


#include <reg52.h>
#include "nrf24L01Node.h"
#include "dht11.h"

// Node ID
#define NODE_ID 181




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


void delay10s(void) 
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 82}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( unsigned char readDHT11Result )
{
	unsigned char sendData[16];
	//unsigned char toAddr[3]= {53, 69, 149}; //Pi, 3�ֽڵ�ַ
	unsigned char toAddr[3]= {53, 70, 132}; //Pi2, 3�ֽڵ�ַ
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = readDHT11Result;
	sendData[2] = getDHT11Humidity();
	sendData[3] = getDHT11Temperature();
	
	
	//tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	tmp = nrfSendData( 92, 3, toAddr, 16, sendData);//Pi2, 92Ƶ����3�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;
	unsigned char readDht11Result = 0;

	
	
	//��ʼ���ж�0
	initINT0();
	
	//init dht11
	initDHT11();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	//initDelay();
			
	while(1)
	{
		//��������������
		if( ++sendDataTimerCount >= 6 )  //�������Ϊ60����ÿ60*10��600�뷢��һ������
		{
			//��ȡDHT11
			readDht11Result = readDHT11(2);
			sendDataToHost( readDht11Result );	
			sendDataTimerCount=0;
		}
		delay10s(); //��ʱ10��
	}
}



//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
}
