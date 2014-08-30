//***************************************
// BMP085 IIC���Գ���
// ʹ�õ�Ƭ��11F04E
// ʱ�䣺2012��9��17��
//****************************************
#include <reg52.h>	 
#include "bmp085.h" 
#include "bh1750fvi.h"
#include "nrf24L01.h"
#include "nrf24L01Node.h"

typedef union
{
	long val;
	unsigned char byte[4];
} MY_LONG;

sfr AUXR = 0x8E;

sbit PIR=P1^7; //���ͺ���̽ͷ���Ŷ���

//��ʱ�ж��ۼ�10ms��1s�ı���
volatile unsigned char count10ms = 0;
volatile unsigned int count1s = 0;

//ʱ�䵽�������ݵı�־λ
volatile bit toSend = 0;

void initTimer0(void)
{
	TMOD = 0x01;
    TH0 = 0x10;
    TL0 = 0x30;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void main()
{ 
	unsigned char sendData[15];
	//unsigned char addr[3];
	bit thisPIR=0, lastPIR=0;
	MY_LONG tmp;

	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	bmp085Init(); //��ʼ��BMP085 
	bh1750Init(); // ��ʼ�� BH1750FVI
	
	initINT0(); //��ʼ���ж�0
	nrf24L01Init(); //��ʼ��NRF24L01+
	//nrfSetRxMode(); //����nrf24L01Ϊ����ģʽ
	
	initTimer0(); //��ʼ��Timer0
	
	while(1) //ѭ��
	{
		//��ȡ����̽ͷ��ǰ״̬
		thisPIR = PIR;
		
//		//���ͺ���̽ͷ���� 100-111
//		if( thisPIR != lastPIR)
//		{
//			lastPIR = thisPIR;
//			
//			//��������������
//			sendData[0] = NODE_ID_0;
//			sendData[1] = 111;
//			sendData[2] = thisPIR;
//			nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
//			
//			//�򱨾���������
//			sendData[0] = 111;
//			sendData[1] = thisPIR;
//			addr[0]=97;
//			addr[1]=83;
//			addr[2]=170;
//			nrfSendData( 82, ADDRESS_WIDTH, addr, 2, sendData);
//
//			
//			//�鷿�ƿ���
//			if( thisPIR && bh1750GetBrightness()<1)
//			{
//				//����
//				sendData[0] = 111;
//				sendData[1] = 1;
//				addr[0]=97;
//				addr[1]=83;
//				addr[2]=175;
//				nrfSendData( 82, ADDRESS_WIDTH, addr, 2, sendData);
//			}
//			else
//			{
//				//�ص�
//				sendData[0] = 111;
//				sendData[1] = 0;
//				addr[0]=97;
//				addr[1]=83;
//				addr[2]=175;
//				nrfSendData( 82, ADDRESS_WIDTH, addr, 2, sendData);
//			}
//
//			//�򱨾����ڵ㴫������
//			//nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
//				
//			//nrfSetRxMode(); //����nrf24L01Ϊ����ģʽ
//
//		}
		
		//��ʱ������ѹ�¶����ȵ����� 100-11
		if( toSend )
		{
			// initialize Node ID
			sendData[0]=NODE_ID_0;
			sendData[1]=NODE_ID_1;
			
			// ������ѹ���¶ȡ���������
			bmp085Convert();
			
			tmp.val = bmp085GetTemperature();
			sendData[2]=tmp.byte[0];
			sendData[3]=tmp.byte[1];
			sendData[4]=tmp.byte[2];
			sendData[5]=tmp.byte[3];
			// ���ݸ�ԭ tmp.byte[0]*16777216 + tmp.byte[1]*65536 + tmp.byte[2]*256 + tmp.byte[3]
			
			tmp.val = bmp085GetPressure();
			sendData[6]=tmp.byte[0];
			sendData[7]=tmp.byte[1];
			sendData[8]=tmp.byte[2];
			sendData[9]=tmp.byte[3];
			// ���ݸ�ԭ tmp.byte[0]*16777216 + tmp.byte[1]*65536 + tmp.byte[2]*256 + tmp.byte[3]
			
			tmp.val = (unsigned long) bh1750GetBrightness();
			sendData[10]=tmp.byte[0];
			sendData[11]=tmp.byte[1];
			sendData[12]=tmp.byte[2];
			sendData[13]=tmp.byte[3];
			// ���ݸ�ԭ tmp.byte[2]*256 + tmp.byte[3]
			
			sendData[14] = thisPIR;
			
			nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
			
			toSend = 0;
			
			//nrfSetRxMode(); //����nrf24L01Ϊ����ģʽ
		}
	}
}

// �ж�ÿ10msһ��
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x1d;
    TL0 = 0x70;
    
	if( ++count10ms == 100 ) // ÿ100��10ms����1��
	{
		TR0=0;
		count10ms = 0;
		if( ++count1s == 600 )
		{
			count1s = 0;
			toSend = 1;
		}
		TR0=1;
	}
}


////�ⲿ�ж�0�������
////���ڴ�������24L01���ж�
//void interrupt24L01(void) interrupt 0
//{
//	unsigned char * receivedData;
//	
//	//��ͣ��ʱ����ֹ�жϻ��ң�
//	TR0 = 0;
//
//	receivedData = nrfGetReceivedData();
//	
//	modeLight = *(receivedData++);
//	modeAlarm = *(receivedData++);
//	
//	switch( modeLight)
//	{
//		case 0: //��
//			controlLight(0);
//			break;
//		
//		case 1: //��
//			controlLight(1);
//			break;	
//		
//		case 2: //�Զ�
//			if( PIR && (bh1750GetBrightness()<1) ) //����������С��1�ſ���
//			{
//				controlLight(1);
//			}
//			else
//			{
//				controlLight(0);
//			}
//	}
//
//	//������ģʽ��Ϊ0�����Ϲرձ�����
//	if( modeAlarm==0 )
//	{
//		controlAlarm(0);
//	}
//	//toSend = 1;
//	
//	nrfSetRxMode(); //����nrf24L01Ϊ����ģʽ
//
//	
//	//������ʱ
//	TR0 = 1;
//}