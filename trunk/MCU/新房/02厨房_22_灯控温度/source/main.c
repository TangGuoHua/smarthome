/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�������PIR̽ͷ
�̵���ģ��
DS18B20�¶�̽ͷ

���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2013��09��26��  �Ƴ���  ��ʼ�汾
2014��01��27��  �Ƴ���  ����initDelay()
2014��02��09��  �Ƴ���  1�š�2�ŵƷֱ����ÿ�����ֵ
2014��10��02��  �Ƴ���  ����nrf24l01+����
                        ����Pi���������ӵƹ���ģʽֵ


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
#include "ds18B20.h"

// Node ID
#define NODE_ID 22

sfr AUXR   = 0x8E;

//sbit LDR = P1^7;  //�������� ��10K������
sbit PIR = P1^3;  //���͵���⴫����

sbit RELAY_LIGHT1 = P1^5; //�ƿؼ̵���1 ����ƣ�
sbit RELAY_LIGHT2 = P1^4; //�ƿؼ̵���2 �����ƣ�


volatile unsigned char light1Mode = 2; //0�����أ�1��������2���Զ�
volatile unsigned char light2Mode = 2; //0�����أ�1��������2���Զ�
volatile unsigned char light1OnThreshold = 100; // ��1�ŵƵ���ֵ
volatile unsigned char light2OnThreshold = 80; // ��2�ŵƵ���ֵ

volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerSendData = 0;

// Flag for sending data to Pi
bit sendDataNow = 0;


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




//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 22, 231, 231}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr); //����92Ƶ����5�ֽڵ�ַ
}


//���ص�ǰ����ֵ
unsigned char getBrightness()
{
	return (255-getADCResult(7));
}



//��������ֵ����Ƭ��eeprom
void saveLightOnThreshold( unsigned char threshold1, unsigned char threshold2 )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, threshold1 );
	eepromWrite( 0x0002, threshold2 );
}

////��Ƭ��eepromȡ�ÿ�����ֵ
//unsigned char getLightOnThreshold()
//{
//	return eepromRead(0x0000);
//}


void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 96Ƶ����5�ֽڵ�ַ������16�ֽ�
	unsigned char tmp;
	float temperature;
	int intTemperature;
		
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update
	sendData[2] = PIR;
	sendData[3] = getBrightness(); //����
	sendData[4] = light1OnThreshold;
	sendData[5] = light2OnThreshold;
	sendData[6] = light1Mode;
	sendData[7] = light2Mode;
	sendData[8] = ~RELAY_LIGHT1;
	sendData[9] = ~RELAY_LIGHT2;

	// DS18B20ȡ���¶�ֵ
	temperature = readTemperature(1);

	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	sendData[10]= temperature<0 ? 1 : 0; //������������λ��1
	sendData[11]= intTemperature/10; //�¶ȵ���������
	sendData[12]= intTemperature%10; //�¶ȵ�С������

	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//��Pi����, 96Ƶ����3�ֽڵ�ַ��16�ֽ�����
	
	//24L01��ʼ��������
	startRecv(); 
}


// ��ʼ��Timer0
void initTimer0(void)
{
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
}

void main()
{

	bit lastPIR=0;
	bit thisPIR=0;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//��ʼ���ж�0
	initINT0();
    
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��DS18B20
	initDS18B20();
	
	//��ʼ��ADC
	initADC();
	
	//ȡ�ÿ�����ֵ
	light1OnThreshold = eepromRead(0x0000);
	light2OnThreshold = eepromRead(0x0002);

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

		//Send data to Pi
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();
		}
		
		// �ƿ�1
		if( light1Mode == 0 ) //����
		{
			RELAY_LIGHT1=1;
		}
		else if( light1Mode ==1 ) //����
		{
			RELAY_LIGHT1=0;
		}
		else if( light1Mode ==2 ) //�Զ�
		{
			if(RELAY_LIGHT1) //��ǰ�������
			{
				if( thisPIR && getBrightness()<=light1OnThreshold ) //���ˣ��һ�����������ֵ����
				{
					RELAY_LIGHT1 = 0; //����
				}
			}
			else //��ǰ��������
			{
				RELAY_LIGHT1 = ~thisPIR; //����������ص�
			}
		}
		
		// �ƿ�2
		if( light2Mode == 0 ) //����
		{
			RELAY_LIGHT2=1;
		}
		else if( light2Mode ==1 ) //����
		{
			RELAY_LIGHT2=0;
		}
		else if( light2Mode ==2 ) //�Զ�
		{
			if(RELAY_LIGHT2) //��ǰ�������
			{
				if( thisPIR && getBrightness()<=light2OnThreshold ) //���ˣ��һ�����������ֵ����
				{
					RELAY_LIGHT2 = 0; //����
				}
			}
			else //��ǰ��������
			{
				RELAY_LIGHT2 = ~thisPIR; //����������ص�
			}
		}		
		
		//��¼��ǰֵ
		lastPIR = thisPIR;

	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char tmp1, tmp2;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): ��1�ŵƵ�������ֵ
	// *(receivedData+1): ��2�ŵƵ�������ֵ
	// *(receivedData+2): �ƿؼ̵���1�Ĺ���ģʽ
	// *(receivedData+3): �ƿؼ̵���2�Ĺ���ģʽ

	
	//������ֵ
	tmp1 = *(receivedData+0);
	tmp2 = *(receivedData+1);
	if( tmp1 != light1OnThreshold || tmp2 != light2OnThreshold )
	{
		//threshold changed, let's save it.
		light1OnThreshold = tmp1;
		light2OnThreshold = tmp2;
		saveLightOnThreshold( tmp1, tmp2 );
	}
	
	light1Mode = *(receivedData+2);
	light2Mode = *(receivedData+3);
	

}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100��10ms����1��
	{
		timerCounter10ms=0;
		
		if( ++timerSendData == 600 ) //ÿ600����Pi����һ������
		{
			timerSendData = 0;
			sendDataNow = 1;
		}
		

	}
}
