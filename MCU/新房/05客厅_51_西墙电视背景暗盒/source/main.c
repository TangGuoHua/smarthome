/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
8λ�̵���ģ�� 

����Ϊ���Ӷ˿ڲ���������1�ż̵������á�
2�ż̵�����Ȼ�����ã����ǲ�������ʵ��û�н���
3�ż̵���ʹ���˳��մ��㡣Ŀǰ�ӵ��ӻ���


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��06��29��  �Ƴ���  ��ʼ�汾

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
#define NODE_ID 51

sbit RELAY_2 = P1^3;
sbit RELAY_3 = P3^5;
sbit RELAY_4 = P1^5;
sbit RELAY_5 = P1^4;
sbit RELAY_6 = P1^7;
sbit RELAY_7 = P1^6;
sbit RELAY_8 = P3^7;

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
	
	RELAY_2=1;
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




//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 51}; //���ڵ�Ľ��յ�ַ
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
	
	// Relay_1 ���ã���2�ſ�ʼ��
	RELAY_2 = (*(receivedData++)==0)?1:0;
	RELAY_3 = (*(receivedData++)==0)?1:0;
	RELAY_4 = (*(receivedData++)==0)?1:0;
	RELAY_5 = (*(receivedData++)==0)?1:0;
	RELAY_6 = (*(receivedData++)==0)?1:0;
	RELAY_7 = (*(receivedData++)==0)?1:0;
	RELAY_8 = (*(receivedData++)==0)?1:0;

}
