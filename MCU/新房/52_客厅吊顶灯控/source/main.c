/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
2λ�̵���ģ��*2
PIRģ��


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��09��05��  �Ƴ���  ��ʼ�汾
2014��12��07��  �Ƴ���  ����NRF����
                        ����PIR̽ͷ
						����eeprom��ֵ

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
#define NODE_ID 52

sbit PIR = P3^7;
sbit RELAY_1 = P1^4; //��
sbit RELAY_2 = P1^5; //��
sbit RELAY_3 = P1^6; //��
sbit RELAY_4 = P1^7; //��


volatile bit gPeopleIn = 0; //����Ϊ1������Ϊ0

//PIR��ʱ�������ж��Ƿ����ˣ���λ������
//������PIR����������ô����Ӻ󣬲��жϿ�������
volatile unsigned char gPIRDelay = 10; 


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
	//40��
    unsigned n;
    for(n=0;n<8;n++)
	{
		delay5s();
	}

}


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
	unsigned char myAddr[5]= {97, 83, 231, 231, 52}; //���ڵ�Ľ��յ�ַ
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

	sendData[2] = ~RELAY_1;
	sendData[3] = ~RELAY_2;
	sendData[4] = ~RELAY_3;
	sendData[5] = ~RELAY_4;
	sendData[6] = PIR;
	sendData[7] = gPeopleIn;
	sendData[8] = gPIRDelay;
	
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
	
	switch( *(receivedData+1) )
	{
		case 22: //ִ�����������
			gFuncNum = 23;
			gSendDataNow = 1;
					
		case 20: //ִ��������践��
			RELAY_1 = (*(receivedData+2)==0)?1:0;
			RELAY_2 = (*(receivedData+3)==0)?1:0;
			RELAY_3 = (*(receivedData+4)==0)?1:0;
			RELAY_4 = (*(receivedData+5)==0)?1:0;
			if( gPIRDelay != *(receivedData+6))
			{
				gPIRDelay = *(receivedData+6);
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
