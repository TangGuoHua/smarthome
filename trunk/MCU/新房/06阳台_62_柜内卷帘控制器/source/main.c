/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�̵���ģ��


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��07��30��  �Ƴ���  ��ʼ�汾


����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
//#include "adc.h"
#include "stcEEPROM.h"
//#include "ds18B20.h"

// Node ID
#define NODE_ID 62

sfr AUXR   = 0x8E;

//sbit LDR = P1^7;  //�������� ��10K������


sbit RELAY_POWER = P1^5; //�̵���1�����Ƶ�Դ
sbit RELAY_DIRECTION = P1^4; //�̵���2�����Ʒ��򣬳���-����������-�½�


unsigned char curtainMode = 1; //1:�ֶ���2���Զ�
unsigned char curtainOpenPercent
bit motorDirection = 0; //����˶�����1������������������0���½����ؾ�����
bit motorOn = 0; //��￪��, 1������ 0����

unsigned char timerCounter10ms = 0;
unsigned int timerSendData = 0;

// Flag for sending data to Pi
bit sendDataNow = 0;


void delay200ms()
{
	//todo
}

void stopMotor()
{
	//ͣ����Դ
	RELAY_POWER = 1;
	
	delay200ms();
	
	//����ѡ��̵����ϵ�
	RELAY_DIRECTION = 1;
}

void startMotor()
{
	//�趨����
	RELAY_DIRECTION = motorDirection;
	
	//ͨ��
	RELAY_POWER = 0;
}

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
	unsigned char myAddr[3]= {97, 83, 62}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr); //����92Ƶ����3�ֽڵ�ַ
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
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
	unsigned char tmp;
	float temperature;
	int intTemperature;
		
	sendData[0] = NODE_ID;//Node ID

	sendData[1] = PIR;
	sendData[2] = getBrightness(); //����
	sendData[3] = light1OnThreshold; 
	sendData[4] = light2OnThreshold; 
		
	sendData[5] = ~RELAY_LIGHT1;
	sendData[6] = ~RELAY_LIGHT2;

	// DS18B20ȡ���¶�ֵ
	temperature = readTemperature(1);

	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	sendData[7]= temperature<0 ? 1 : 0; //������������λ��1
	sendData[8]= intTemperature/10; //�¶ȵ���������
	sendData[9]= intTemperature%10; //�¶ȵ�С������

	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//��Pi����, 96Ƶ����3�ֽڵ�ַ��16�ֽ�����
	
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
		if( motorOn && (RELAY_POWER==1) ) //˵�������ǰ���ڹ���
		{
			startMotor( 
		}


	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char rMode, rOpenPercent;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): ������NodeID
	// *(receivedData+1): ���ܺ�
	// *(receivedData+2): ��������ģʽ
	// *(receivedData+3): ����

	
	//������ֵ
	rMode = *(receivedData+0);
	rOpenPercent = *(receivedData+1);
	
	
	if( rOpenPercent != curtainOpenPercent )
	{
		if( rOpenPercent > curtainOpenPercent )
		{
			//������
			motorDirection = 1;
			motorOn = 1;
		}
		else
		{
			//�ش���
			motorDirection = 0;
			motorOn = 1;
		}
	}
	

	

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
