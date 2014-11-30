/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
�̵���ģ��10A*2·
AM2321

���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��03��29��  �Ƴ���  ��ʼ�汾
2014��10��12��  �Ƴ���  ����nrf����
                        ����AM2321
						������ʼ��ʱ
2014��10��12��  �Ƴ���  ����ȡů���¿�

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
#define NODE_ID 53

sfr AUXR   = 0x8E;

sbit RELAY_LAMP = P1^5;
sbit RELAY_NOT_USED = P1^4; //û���õ��ļ̵���

sbit RELAY_HEATER1 = P1^7;
sbit RELAY_HEATER2 = P1^6; 


unsigned char gSendDataNow = 0; //���Ϸ�������

unsigned char gHeaterMode = 0;  //0�����أ�1��������2���¿�
unsigned char gSetTemperatureX10 = 0; //�趨�¶�

//��ʱ5�� STC11F04E 4MHz
//��������Ŀ�����ʱ�����ã�
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
	//30��
    unsigned n;
    for(n=0;n<6;n++)
	{
		delay5s();
	}

}


//��ʱ10�� 
void delay10s(void) 
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//�������ر�ȡů��
// onOff = 1������
// onOff = 0���ر�
void switchHeater( unsigned char onOff )
{
	if( onOff==1 ) //switch on
	{
		RELAY_HEATER1=0;
		RELAY_HEATER2=0;
	}
	else if( onOff == 0 ) // switch off
	{
		RELAY_HEATER1=1;
		RELAY_HEATER2=1;	
	}
}


void initRelays()
{
	//��ʼ���̵���
	RELAY_LAMP = 1;
	RELAY_NOT_USED = 1;
	switchHeater(0);
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 53, 231, 53}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( unsigned char functionNum, unsigned char readAm2321Result )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3�ֽڵ�ַ
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = functionNum;// Function Number, 1-regular status update, 51-respond to 50
	sendData[2] = readAm2321Result;
	sendData[3] = getAM2321Data(0);
	sendData[4] = getAM2321Data(1);
	sendData[5] = getAM2321Data(2);
	sendData[6] = getAM2321Data(3);
	sendData[7] = getAM2321Data(4);
	sendData[8] = ~RELAY_LAMP;
	sendData[9] = gHeaterMode;
	sendData[10] = gSetTemperatureX10;
	sendData[11] = ~RELAY_HEATER1;
	sendData[12] = ~RELAY_HEATER2;
	
	tmp = nrfSendData(96, 5, toAddr, 16, sendData);
	
	//24L01��ʼ��������
	startRecv();
}


void main()
{
	unsigned char readAm2321Result = 0;
	int temperatureX10 = 0;
	unsigned char timerCounter10s = 0;
	
	//��ʼ���̵���
	initRelays();
	
	//init AM2321
	initAM2321();	
	
	//��ʼ���ж�0
	initINT0();
	
	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();


	while(1)
	{
		
		//����AM3231����������ȡ��һ�β�������ʪ��ֵ
		readAm2321Result = readAM2321(1);


		if( readAm2321Result == 0 ) //��ȡam2321�ɹ�
		{
			// �������Զ��¿�ģʽ
			if(gHeaterMode==2)
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
					switchHeater(0); //�ؼ̵�����ֹͣ����
					//sendDataToHost( temperatureX10, 0 );
				}
				else if( temperatureX10<=(gSetTemperatureX10-1) ) //�����趨ֵ0.1��
				{
					switchHeater(1); //���̵�������ʼ����
					//sendDataToHost( temperatureX10, 0 );
				}
			}
			
			//��������������
			if( gSendDataNow || ++timerCounter10s >= 60 )  //ÿ60*10��600�뷢��һ������
			{
				if( gSendDataNow )
				{
					gSendDataNow = 0;
					sendDataToHost( 51, readAm2321Result ); //����״̬���� functionNumber=51
				}
				else
				{
					sendDataToHost( 1, readAm2321Result ); //��ʱ״̬���� functionNumber=1
				}
				
				//timer����
				timerCounter10s=0;
			}
		}
		else
		{
			//AM2321��ȡ����
			sendDataToHost( 9, readAm2321Result ); //�����쳣����
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
	
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 20: //ִ��������践��
			
			//�ֽ�2��������صƼ̵�������ģʽ��0�����أ�1��������
			RELAY_LAMP = (*(receivedData+2)==0)?1:0;

			//�ֽ�3��������͡�̵����Ĺ���ģʽ ��0�����أ�1��������2���¿أ�
			gHeaterMode = *(receivedData+3);
			if( gHeaterMode == 0 ) switchHeater(0);
			else if( gHeaterMode == 1 ) switchHeater(1);

			//�ֽ�2���趨�¶� 10����
			//ֻ���趨0-255����0����25.5�ȣ�֮���ֵ
			//���ǵ����ڵ�ֻ���ڶ��������͡������趨�¶�����Ӧ���Ǻ����
			gSetTemperatureX10 = *(receivedData+4);
			break;


		case 50: //������������״̬
			//�ر�50, functionNum = 51;
			gSendDataNow = 1;
		
			break;
	
	}//end of switch
}
