/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�̵���ģ�� 2��


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2013��11��08��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"

sbit RELAY_LIGHT_E = P1^3; //�ƿؼ̵��� ��
sbit RELAY_LIGHT_S = P3^5; //�ƿؼ̵��� ��
sbit RELAY_LIGHT_W = P1^7; //�ƿؼ̵��� ��
sbit RELAY_LIGHT_N = P1^6; //�ƿؼ̵��� ��

//��ʼ��INT0 
//nrf24L01������Ҫ�õ�
void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 41}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr); //����92Ƶ����3�ֽڵ�ַ
}



//��ʼ���̵���
void initRelays()
{
	RELAY_LIGHT_E=1;
	RELAY_LIGHT_S=1;
	RELAY_LIGHT_W=1;
	RELAY_LIGHT_N=1;
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
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	RELAY_LIGHT_E = (*(receivedData+0))==1?0:1 ;
	RELAY_LIGHT_S = (*(receivedData+1))==1?0:1 ;
	RELAY_LIGHT_W = (*(receivedData+2))==1?0:1 ;
	RELAY_LIGHT_N = (*(receivedData+3))==1?0:1 ;

}