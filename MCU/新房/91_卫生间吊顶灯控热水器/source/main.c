/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�������PIR̽ͷ
�̵���ģ��
��������

���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2013��10��01��  �Ƴ���  ��ʼ�汾
2014��01��27��  �Ƴ���  ����initDelay()
2014��10��03��  �Ƴ���  ������92�Žڵ㷢�����ݵ��߼�
                        ����NRF24L01+����
2014��10��06��  �Ƴ���  �޸�initDelay()��ʱΪ(NodeID*2)��
					
����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "adc.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 91

sfr AUXR   = 0x8E;

sbit LIGHT_R = P1^4;  //�������� ��10K������
sbit PIR = P3^5;      //���͵���⴫����
sbit RELAY_HEATER = P3^7; //��ˮ���̵�����30A��
sbit RELAY_LIGHT1 = P1^7; //�ƿؼ̵���1
sbit RELAY_LIGHT2 = P1^6; //�ƿؼ̵���2


volatile unsigned char gLight1Mode = 2; //0�����أ�1��������2���Զ�
volatile unsigned char gLightOnThreshold = 90; // ���Ƶ���ֵ

volatile unsigned char gHeaterMode = 0; //0�����أ�1������

volatile unsigned char gTimerCounter10ms = 0;
volatile unsigned int gTimerSendData = 0;

// Flag for sending data to Pi
volatile bit gSendDataFunctNum = 0;



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
	//10��
    unsigned n;
    for(n=0;n<2;n++)
	{
		delay5s();
	}

}



void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 91, 231, 91}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//���ص�ǰ����ֵ
unsigned char getBrightness()
{
	return (255-getADCResult(4));
}


//��������ֵ����Ƭ��eeprom
void saveLightOnThreshold( unsigned char x )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, x );
}


//��Ƭ��eepromȡ�ÿ�����ֵ
unsigned char getLightOnThreshold()
{
	return eepromRead(0x0000);
}


//�������ݸ�Pi
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1] = gSendDataFunctNum; //Function Number

	sendData[2] = PIR;
	sendData[3] = getBrightness(); //����
	sendData[4] = gLightOnThreshold; 
		
	sendData[5] = ~RELAY_LIGHT1;
	sendData[6] = ~RELAY_LIGHT2;
	sendData[7] = ~RELAY_HEATER;
	
	//Stop timer0
	TR0=0;
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96Ƶ����5�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv();
	
	//Start timer0
	TR0=1;
}


//�������ݸ�92�Žڵ�
//������С������
void sendDataToNode92( unsigned char val )
{
	unsigned char sendData[6];
	unsigned char toAddr[5]= {97,83,92,231,92}; //Node 92��ַ
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1]= 2; //�ض��¼�����
	sendData[2]= 0; //������accident��
	sendData[3]= val;

	//Stop timer0
	TR0=0;
	
	tmp = nrfSendData( 92, 5, toAddr, 6, sendData);//Pi, 92Ƶ����5�ֽڵ�ַ��6�ֽ�����
	
	//24L01��ʼ��������
	startRecv();
	
	//Start timer0
	TR0=1;
	
}


// ��ʼ��Timer0
void initTimer0(void)
{
	AUXR = AUXR|0x80;  // T0, 1T Mode
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}


void initRelays()
{
	RELAY_LIGHT1=1;
	RELAY_LIGHT2=1;
	RELAY_HEATER=1;
}


void main()
{

	bit lastPIR=0;
	bit thisPIR=0;
	
	//��ʼ���ж�0
	initINT0();
    
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��ADC
	initADC();
	
	//ȡ�ÿ�����ֵ
	gLightOnThreshold = getLightOnThreshold();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
	
	//��ʼ��timer0
	initTimer0();

	while(1)
	{
		//ȡ�ô������ĵ�ǰֵ
		thisPIR = PIR;

		// �ƿ�1
		if( gLight1Mode == 0 ) //����
		{
			RELAY_LIGHT1=1;
		}
		else if( gLight1Mode ==1 ) //����
		{
			RELAY_LIGHT1=0;
		}
		else if( gLight1Mode ==2 ) //�Զ�
		{
			if(RELAY_LIGHT1) //��ǰ�������
			{
				if( thisPIR && getBrightness()<=gLightOnThreshold ) //���ˣ��һ�����������ֵ����
				{
					RELAY_LIGHT1 = 0; //����
				}
				
			}
			else //��ǰ��������
			{
				RELAY_LIGHT1 = ~thisPIR; //����������ص�
			}
		}
		
		//����������״̬����ת��ʱ�������ݴ���92�Žڵ�
		//���ڿ���С������NodeID:92��		
		if( thisPIR != lastPIR ) 
		{
			sendDataToNode92( thisPIR );
		}
		
		//��¼��ǰֵ
		lastPIR = thisPIR;
		
		
		//Send data to Pi
		if( gSendDataFunctNum )
		{
			sendDataToHost(  );
			gSendDataFunctNum = 0;
		}

	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char fromNodeID, funcNum;
	unsigned char tmp;
	
	//Stop timer0
	TR0=0;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	//Start timer0
	TR0=1;
	
	// *(receivedData+0): From Node ID, �̶�Ϊ1
	// *(receivedData+1): Function Number
	// *(receivedData+2): ���Ƶ�������ֵ
	// *(receivedData+3): �ƿؼ̵���1�Ĺ���ģʽ
	// *(receivedData+4): �ƿؼ̵���2�Ĺ���ģʽ
	// *(receivedData+5): ��ˮ������ģʽ
	// *(receivedData+6): ��ˮ����ˮ������
	

	
	fromNodeID = *receivedData;
	funcNum = *(receivedData+1);
	
	if( fromNodeID == 1 )//����Pi(#1)
	{
		if( funcNum == 20 || funcNum == 22)
		{
	
			//������ֵ
			tmp = *(receivedData+2);
			if( tmp!= gLightOnThreshold )
			{
				//threshold changed, let's save it.
				gLightOnThreshold = tmp;
				saveLightOnThreshold( gLightOnThreshold );
			}
			
			//�ƿ�1��ģʽ
			gLight1Mode = *(receivedData+3);
			
			//�ƿ�2��ʱû��
			
			//��ˮ������ģʽ
			tmp = *(receivedData+5);
			if( tmp != gHeaterMode ) //ģʽ�仯��
			{
				if( tmp==0 )
				{
					//��
					RELAY_HEATER = 1;
				}
				else if( tmp==1 )
				{
					//��
					RELAY_HEATER = 0;
				}
				else if( tmp==2 )
				{
					//��ʱ
				}
				gHeaterMode = tmp;
			}
		}
		
		if( funcNum == 22 )
		{
			gSendDataFunctNum = 23;
		}
		else if( funcNum == 50 )
		{
			gSendDataFunctNum = 51;
		}		
	}
}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++gTimerCounter10ms == 100 ) //100��10ms����1��
	{
		gTimerCounter10ms=0;
		
		if( ++gTimerSendData == 600 ) //ÿ600����Pi����һ������
		{
			gTimerSendData = 0;
			gSendDataFunctNum = 1;
		}
	}
}

