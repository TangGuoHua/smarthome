/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
2λ�̵���ģ��*2


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��09��05��  �Ƴ���  ��ʼ�汾

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
#define NODE_ID 52

sbit RELAY_1 = P1^4;
sbit RELAY_2 = P1^5;
sbit RELAY_3 = P1^6;
sbit RELAY_4 = P1^7;

//������ʱ 
//����NodeID������ԼΪ(500*NodeID)�������ʱ
//�����Ǳ������нڵ�ͬʱ�ϵ磬������5���Ӽ������������ɵ�ͨѶ��ײ
//void initDelay(void)
//{
//	//4MHz Crystal, 1T STC11F04E
//    unsigned char a,b,c,d;
//    for(d=NODE_ID;d>0;d--)
//	    for(c=167;c>0;c--)
//	        for(b=171;b>0;b--)
//	            for(a=16;a>0;a--);
//}

//��ʼ���̵���
void initRelays()
{
	RELAY_1=1;
	RELAY_2=1;
	RELAY_3=1;
	RELAY_4=1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 52}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr);
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
	
	
	while(1)
	{
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
	RELAY_2 = (*(receivedData++)==0)?1:0;
	RELAY_3 = (*(receivedData++)==0)?1:0;
	RELAY_4 = (*(receivedData++)==0)?1:0;
}
