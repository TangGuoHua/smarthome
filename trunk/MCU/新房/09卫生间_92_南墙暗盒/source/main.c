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

// Node ID
#define NODE_ID 92


sbit RELAY_WATERHEATER = P3^7;  //С������ˮ�����Ƽ̵���


float gTemperature; //DS18B20��õ��¶�

//������ʱ 
//����NodeID������ԼΪ(500*NodeID)�������ʱ
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
	unsigned char myAddr[3]= {97, 83, 92}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 3�ֽڵ�ַ
	unsigned char tmp;
	
	int intTemperature;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = (~RELAY_WATERHEATER); //Water heater relay status
	
	// DS18B20ȡ���¶�ֵ
	//temperature = readTemperature(1);

	//�������뵽ʮ��λ
	intTemperature = gTemperature<0? (gTemperature*(-10)+0.5) : gTemperature*10+0.5; 
	
	sendData[2]= gTemperature<0 ? 1 : 0; //������������λ��1
	sendData[3]= intTemperature/10; //�¶ȵ���������
	sendData[4]= intTemperature%10; //�¶ȵ�С������
	
	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;

	//��ʼ���ж�0
	initINT0();
	
	//��ʼ��DS18B20
	initDS18B20();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
			
	while(1)
	{
		// DS18B20ȡ���¶�ֵ
		gTemperature = readTemperature(0);	
		
		//��������������
		if( ++sendDataTimerCount >= 60 )  //�������Ϊ60����ÿ60*10��600�뷢��һ������
		{
			sendDataToHost();	
			sendDataTimerCount=0;
		}
		
		delay10s(); //��ʱԼ10��
	}
}



//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	//�ֽ�1������1�ż̵�������ģʽ��0�����أ�1��������2����ʱ�أ�
	RELAY_WATERHEATER = (*(receivedData++)==0)?1:0;

}
