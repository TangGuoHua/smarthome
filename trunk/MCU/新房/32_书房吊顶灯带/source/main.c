/***************************************************************************
��Ӳ����Ϣ��
��Ƭ��: STC11F04E, 1T, �ڲ�RC��
������
�̵���ģ�� 2��


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


#include<reg52.h>
#include "nrf24L01Node.h"

// Node ID
#define NODE_ID 83

sfr AUXR = 0x8E;

sbit OUT_RELAY=P1^6;  //�̵������ƶ� 1-off, 0-on

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void initRelays()
{
	//��ʼ���̵���
	OUT_RELAY = 1;
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 32, 231, 32}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


main()
{
	
	//��ʼ���̵���
	initRelays();
		
	//��ʼ���ж�0
	initINT0();
	
	//��ʼ��nrf
	nrf24L01Init();
	
	startRecv(); //��ʼ����
	
	while(1)
	{

	}
}

//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	
	receivedData = nrfGetReceivedData();
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 20: //ִ��������践��
			
			//���Ƽ̵�������ģʽ��0�����أ�1��������
			OUT_RELAY = (*(receivedData+2)==0)?1:0;

		
		case 50: //������������״̬
		
			break;
	
	}//end of switch
		
	
	//�ٴν������ģʽ
	//nrfSetRxMode();
}