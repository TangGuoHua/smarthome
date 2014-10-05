/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
����AM2321��ʪ�ȴ�����
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
#include "am2321.h"

// Node ID
#define NODE_ID 82

sbit RELAY_HEATER = P3^7;
unsigned char gRelayMode = 0;  //0�����أ�1��������2���¿�
unsigned char gSetTemperatureX10; //�趨�¶�


//������ʱ 
//����NodeID������ԼΪ500*NodeID�������ʱ
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
	unsigned char myAddr[3]= {97, 83, 82}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( unsigned char readAm2321Result )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 3�ֽڵ�ַ
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = readAm2321Result;
	sendData[2] = getAM2321Data(0);
	sendData[3] = getAM2321Data(1);
	sendData[4] = getAM2321Data(2);
	sendData[5] = getAM2321Data(3);
	sendData[6] = getAM2321Data(4);
	
	sendData[7] = ~RELAY_HEATER;
	sendData[8] = gRelayMode;
	sendData[9] = gSetTemperatureX10;
	
	
	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;
	unsigned char readAm2321Result = 0;
	int temperatureX10 = 0;
	
	RELAY_HEATER=1;
	
	
	//��ʼ���ж�0
	initINT0();
	
	//init 2321
	initAM2321();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
			
	while(1)
	{
		
		//����AM3231����������ȡ��һ�β�������ʪ��ֵ
		readAm2321Result = readAM2321();


		if( readAm2321Result == 0 ) //��ȡam2321�ɹ�
		{
			// �������Զ��¿�ģʽ
			if(gRelayMode==2)
			{
				//��õ�ǰ�¶� 10����
				temperatureX10 = getAM2321Data(2)*10 + getAM2321Data(3);
				if( getAM2321Data(4) == 1 )
				{
					//���¶�
					temperatureX10 *= -1;
				}
				
				if( temperatureX10>=(gSetTemperatureX10+1) ) //�����趨ֵ0.1��
				{
					RELAY_HEATER = 1; //�ؼ̵�����ֹͣ����
					//sendDataToHost( temperatureX10, 0 );
				}
				else if( temperatureX10<=(gSetTemperatureX10-1) ) //�����趨ֵ0.1��
				{
					RELAY_HEATER = 0; //���̵�������ʼ����
					//sendDataToHost( temperatureX10, 0 );
				}
			}	
			
			//��������������
			if( ++sendDataTimerCount >= 60 )  //�������Ϊ60����ÿ60*10��600�뷢��һ������
			{
				sendDataToHost( readAm2321Result );	
				sendDataTimerCount=0;
			}
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
	
	//�ֽ�1��������͡�̵����Ĺ���ģʽ ��0�����أ�1��������2���¿أ�
	gRelayMode = *(receivedData++);
	if( gRelayMode == 0 ) RELAY_HEATER=1;
	else if( gRelayMode == 1 ) RELAY_HEATER=0;
	
	//�ֽ�2���趨�¶� 10����
	//ֻ���趨0��255����0����25.5�ȣ�֮���ֵ
	//���ǵ����ڵ�ֻ���ڶ��������͡������趨�¶�����Ӧ���Ǻ����
	gSetTemperatureX10 = *(receivedData++);
}
