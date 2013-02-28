// STC12C5608AD 1T��Ƭ��  4M�ⲿ������

#include<reg52.h>
//#include <intrins.h>

#include "nrf24L01Node.h"
#include "adc.h"
#include "am2301.h"
//#include "com.h"



//�жϼ�������ÿ50ms��һ
unsigned char timerMSCounter = 0;

//�жϼ�������ÿ1���һ
unsigned int timerSecCounter = 0;

//�����������ݼ��
unsigned int sendDataInterval = 60;
bit sendDataFlag = 1;


//void initINT0(void)
//{
//	EA=1;
//	EX0=1; // Enable int0 interrupt.
//}

void initTimer0(void)
{
	TMOD &= 0xf0; //�����ʱ��0ģʽλ
	TMOD |= 0x01; //�趨��ʱ��0Ϊ��ʽ1��16λ������


	//TH0=0x8f; //����ʱ������ֵ����ʱʱ��50ms���ڲ�RC��
	//TL0=0x00; 
	
	TH0 = 0x0Bf;  //����ʱ������ֵ����ʱʱ��50ms��1T STC12C5608AD 4M����
    TL0 = 0x085;


	ET0=1; //����ʱ 0 �ж� 
	EA=1; //�������ж� 
	TR0=1; //������ʱ�� 0 
	
	//�������
	timerMSCounter = 0;
	timerSecCounter = 0;
	

}

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M����
}



void sendData()
{
	unsigned char txData[HOST_DATA_WIDTH];
	
	unsigned long sum;
	unsigned int i;
	unsigned char vol;
	
	sum=0;
	for( i=0; i<20000; i++ )
	{
		sum += getADCResult(0);
	}
	vol=(unsigned char) (sum/i);

	
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	
	txData[2] = vol;
	
	txData[3] = readAM2301(2);
	txData[4] = getAM2301Data(0);
	txData[5] = getAM2301Data(1);
	txData[6] = getAM2301Data(2);
	txData[7] = getAM2301Data(3);
	txData[8] = getAM2301Data(4);
	
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
}

void main( void )
{
	
	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ2�룬�ô󲿷�Ԫ���ȶ�����
	delayMS( 2000 );
	
	// initialize ADC
	initADC();
	
	// initialze UART
	//serialPortInit();
	
	//initialize 24L01
	nrf24L01Init();
	
	// initialize AM2301
	initAM2301();
		
	//��ʼ����ʱ��0
	initTimer0();
	
	
	while(1)
	{
		if( sendDataFlag )
		{
			//���ʶλ
			sendDataFlag = 0;
			
			//�������ݸ�����
			sendData();
		}
	}
}

//��ʱ��0�жϴ������
void interruptTimer0(void) interrupt 1
{
	TR0=0; //�رն�ʱ��0
	ET0=0; //�رն�ʱ��0�ж�

	TH0 = 0x0Bf; //����ʱ��0�ظ���ֵ����ʱʱ��50ms 4M���� 1T STC12C5608AD
    TL0 = 0x0a5; //��װ��8λ���������ֵ��

	
	TR0=1; //�򿪶�ʱ��0

	if( ++timerMSCounter==20 ) //ÿ20���ж���1�룬����50ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter == sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //�򿪶�ʱ��0�ж�
}
