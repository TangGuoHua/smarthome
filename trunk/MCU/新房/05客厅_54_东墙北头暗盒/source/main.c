/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��01��26��  �Ƴ���  ��ʼ�汾
2014��01��27��  �Ƴ���  ����initDelay()

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

//sbit RELAY_1 = P1^4;
//sbit RELAY_2 = P1^5;
//unsigned char relay1Mode = 0;
//unsigned char relay2Mode = 0;
//unsigned char relay1DelayOffTimerCount = 0; //1�ż̵�����ʱ�� 10s������
//unsigned char tUpper, tLower; //�¶�������


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

//
////NRF24L01��ʼ�������ģʽ
//void startRecv()
//{
//	unsigned char myAddr[3]= {97, 83, 192}; //���ڵ�Ľ��յ�ַ
//	nrfSetRxMode( 92, 3, myAddr);
//}

////return �¶ȵ�10�����¶��������뵽0.1��
////���磺����¶�12.355���򷵻�124��
////      ����¶�0.215���򷵻�2��
//int getTemperatureX10()
//{
//	float temperature;
//	int intTemperature;
//	
//	temperature = readTemperature();
//	
//	//�������뵽ʮ��λ
//	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
//	return intTemperature;
//}

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
	
	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	//startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;
	unsigned char readAm2321Result = 0;
	int temperatureX10 = 0;
	
//	RELAY_1=1;
//	RELAY_2=1;
	
	
	//��ʼ���ж�0
	initINT0();
	
	//init 2321
	initAM2321();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	//startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
			
	while(1)
	{
		
		//����AM3231����������ȡ��һ�β�������ʪ��ֵ
		readAm2321Result = readAM2321();


//		if( readAm2321Result == 0 ) //��ȡam2321�ɹ�
//		{
//			//��õ�ǰ�¶� 10����
//			temperatureX10 = getAM2321Data(2)*10 + getAM2321Data(3);
//			if( getAM2321Data(4) == 1 )
//			{
//				//���¶�
//				temperatureX10 *= -1;
//			}
//			
//			// �������Զ��¿�ģʽ
//			if(relay2Mode==2)
//			{
//				if( temperatureX10>=tUpper )
//				{
//					RELAY_2 = 1; //�ؼ̵�����ֹͣ����
//					//sendDataToHost( temperatureX10, 0 );
//				}
//				else if( temperatureX10<=tLower )
//				{
//					RELAY_2 = 0; //���̵�������ʼ����
//					//sendDataToHost( temperatureX10, 0 );
//				}
//			}	
//		}
//		
//
//		//�̵�����ʱ��
//		if( relay1Mode == 2 && RELAY_1 == 0 )
//		{
//			if( relay1DelayOffTimerCount < 9 ) //�������Ϊ9������ʱ9*10��90 �� 100��ر�1�ż̵���
//			{
//				relay1DelayOffTimerCount++;
//			}
//			else
//			{
//				RELAY_1 = 1; //�ؼ̵���1
//			}
//		}
		
		
		//��������������
		if( ++sendDataTimerCount >= 60 )  //�������Ϊ60����ÿ60*10��600�뷢��һ������
		{
			sendDataToHost( readAm2321Result );	
			sendDataTimerCount=0;
		}
		
		
		delay10s(); //��ʱԼ10��
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
