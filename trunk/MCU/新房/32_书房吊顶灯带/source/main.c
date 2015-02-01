/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
10A*2�̵���ģ��
PIRģ��


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2015��02��01��  �Ƴ���  ����Ӳ�������ư��Ϊv2
                        ����PIR̽ͷ
						����eeprom��ֵ
						�ƴ������Զ�ģʽ

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

//STC11F04E 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"
#include "stcEEPROM.h"

sfr AUXR = 0x8E;

// Node ID
#define NODE_ID 32

sbit PIR = P3^7;

sbit RELAY_1 = P1^7;
sbit RELAY_2 = P1^6; // Ŀǰû���õ�

//LED�ƴ�ģʽ
volatile unsigned char gLEDStripMode = 0; //0�����أ�1��������2���Զ�


volatile bit gPeopleIn = 0; //����Ϊ1������Ϊ0

//PIR��ʱ�������ж��Ƿ����ˣ���λ������
//������PIR����������ô����Ӻ󣬲��жϿ�������
volatile unsigned char gPIRDelay = 20; 


volatile unsigned char gTimerCounter10ms = 0;
volatile unsigned int gCounterRegularStatusReport = 0; //��ʱ״̬�����ʱ��
volatile unsigned int gCounterNoPeopleSeconds = 0; //���˵�ʱ�䣬��λ����

volatile bit gNoPeopleCounterStart = 0; // 1:��ʱ��û���˵�ʱ�䣬 0������ʱ

volatile bit gSendDataNow=0; //�������������ݵı�־λ
volatile unsigned char gFuncNum=0; //���ܺ�

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
	//35��
    unsigned n;
    for(n=0;n<7;n++)
	{
		delay5s();
	}

}


//��ʼ���̵���
void initRelays()
{
	RELAY_1=1;
	RELAY_2=1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void initTimer0(void)
{
	TMOD = 0x01;
	TH0 = 0x10;
	TL0 = 0x30;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 32, 231, 32}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//�������ݸ�Pi
void sendDataToHost()
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = gFuncNum; //Function Number


	sendData[2] = PIR;
	sendData[3] = gPeopleIn;
	sendData[4] = gPIRDelay;
	sendData[5] = gLEDStripMode;
	sendData[6] = ~RELAY_1;
	sendData[7] = ~RELAY_2;
	
	TR0=0;
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96Ƶ����5�ֽڵ�ַ������16�ֽ�
	TR0=1;
	
	//24L01��ʼ��������
	startRecv(); 
}


//��PIR�����ж���ʱ����������Ƭ��eeprom
void savePIRDelayMinute( unsigned char val )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, val );
}


void main()
{

	bit thisPIR=0;
	bit lastPIR=0;
	
	bit thisPeopleIn=0;
	bit lastPeopleIn=0;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode	
	
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��INT0 
	initINT0();

	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv();
	
	//��ʼ��ʱ
	initDelay();
	
	//ȡ��PIR�����ж���ʱ������
	gPIRDelay = eepromRead(0x0000);
	
	//��ʼ��Timer0
	initTimer0();
	
	while(1)
	{
		thisPIR = PIR;
		
		if( thisPIR != lastPIR )
		{
			//�б仯
			
			if( thisPIR )
			{
				//PIR����
				gNoPeopleCounterStart = 0; //ֹͣ��ʱ
				gCounterNoPeopleSeconds = 0;
				
				if( !gPeopleIn )
				{
					//����->����
					gPeopleIn = 1;
					
					gFuncNum = 2;//�����¼�����
					gSendDataNow = 1;

				}
			}
			else
			{
				//����->����
				gNoPeopleCounterStart = 1; //��ʼ��ʱ
				gCounterNoPeopleSeconds = 0;
				
			}
			
			lastPIR = thisPIR;
		}
		
		
		//����ƴ����Զ�ģʽ
		if( gLEDStripMode == 2 )
		{
			RELAY_1 = ~gPeopleIn;
		}

		
		//�������ݸ�����
		if(gSendDataNow)
		{
			sendDataToHost();
			gSendDataNow = 0;
			gCounterRegularStatusReport=0;//��ʱ״̬����Ҳ���¿�ʼ
		}

	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	TR0=0;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	TR0=1;
		
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	//*(receivedData+2) �����ж���ʱ�����ӣ�
	//*(receivedData+3) �ƴ�״̬ 1��������2�����أ�3-�Զ�
	
	switch( *(receivedData+1) )
	{
		case 22: //ִ�����������
			gFuncNum = 23;
			gSendDataNow = 1;
					
		case 20: //ִ��������践��
			gLEDStripMode = *(receivedData+3);

			if( gLEDStripMode == 1 ) //����
			{
				RELAY_1 = 0;
			}
			else if( gLEDStripMode == 0 ) //����
			{
				RELAY_1 = 1;
			}
			
			if( gPIRDelay != *(receivedData+2))
			{
				gPIRDelay = *(receivedData+2);
				savePIRDelayMinute( gPIRDelay );
			}
			break;

		case 50: //������������״̬
			//�ر�50, functionNum = 51;
			gFuncNum = 51;
			gSendDataNow = 1;
			gCounterRegularStatusReport=0;
			break;
	
	}//end of switch
}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++gTimerCounter10ms == 100 ) //100��10ms����1��
	{
		gTimerCounter10ms=0;
		
		//��ʱ״̬����
		if( ++gCounterRegularStatusReport == 600 ) //ÿ600����Pi����һ������
		{
			gCounterRegularStatusReport = 0;
			gFuncNum = 1; //���Ͷ�ʱ״̬����
			gSendDataNow = 1;
		}
		
		//PIR���˼�ʱ
		if( gNoPeopleCounterStart ) 
		{
			if( ++gCounterNoPeopleSeconds >= gPIRDelay*60 )
			{
				//���˵�ʱ�乻���ˣ�����ж�Ϊ������
				
				gNoPeopleCounterStart = 0; //ֹͣ��ʱ
				gCounterNoPeopleSeconds = 0; //����������
				gPeopleIn = 0; //�ж�Ϊ����
				
				gFuncNum = 2;//�����¼�����
				gSendDataNow = 1;			
			}
		}
		

	}
}
