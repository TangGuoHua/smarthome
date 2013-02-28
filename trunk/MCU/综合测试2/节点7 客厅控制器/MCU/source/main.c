// STC11F04E 1T��Ƭ��  �ڲ�RC��

#include <reg52.h>
#include <intrins.h>

#include "nrf24L01Node.h"
#include "am2301.h"

sfr AUXR   = 0x8E;

sbit SENSOR=P1^7;  // ���͵�����ߴ����� PIR
sbit RELAY_1=P3^7; //�̵���1��2
sbit RELAY_2=P1^1;
sbit POWER_METER = P1^0; //���ܱ�ģ���������� 1600������Ϊ1�ȵ�

//�жϼ�������ÿ50ms��һ
unsigned char timerMSCounter = 0;

//�жϼ�������ÿ1���һ
unsigned int timerSecCounter = 0;

//�����������ݼ��
unsigned int sendDataInterval = 300;
bit sendDataFlag = 0;

//���ܱ��������
union { 
	unsigned long longVal;
	unsigned char byteVal[4];
	} powerMeterCount;


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}

void initTimer0(void)
{
	TMOD &= 0xf0; //�����ʱ��0ģʽλ
	TMOD |= 0x01; //�趨��ʱ��0Ϊ��ʽ1��16λ������


	//TH0=0x8f; //����ʱ������ֵ����ʱʱ��50ms���ڲ�RC��
	//TL0=0x00; 
	
	TH0 = 0x77; // 5ms
    TL0 = 0x48;

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
		for( i=530; i>0; i-- ); // 1T STC11F04E �ڲ�RC 6.97MHz
}


//ͨ��24L01��AM2301���ݷ��͸�����
void sendAM2301Data()
{
	unsigned char txData[HOST_DATA_WIDTH];
	
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	
	txData[2] = readAM2301(2);
	txData[3] = getAM2301Data(0);
	txData[4] = getAM2301Data(1);
	txData[5] = getAM2301Data(2);
	txData[6] = getAM2301Data(3);
	txData[7] = getAM2301Data(4);
	txData[8] = powerMeterCount.byteVal[0];
	txData[9] = powerMeterCount.byteVal[1];
	txData[10] = powerMeterCount.byteVal[2];
	txData[11] = powerMeterCount.byteVal[3];
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
}

//void sendPIRData()
//{
//	unsigned char txData[HOST_DATA_WIDTH];
//
//	txData[0] = NODE_ID_0; //Node ID0
//	txData[1] = NODE_ID_1; //Node ID1
//	
//	txData[2] = 5;
//	txData[3] = 6;
//	txData[4] = 7;
//	txData[5] = 8;
//	txData[6] = 9;
//		
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
//
//}

void main( void )
{
	//PIR sensor״̬����
	//bit thisSensor=0, lastSensor=0;
	
	// ���ܱ�ģ������״̬����
	bit thisPowerMeter=1, lastPowerMeter=1;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	
	//�ض������̵���
	RELAY_1 = 1;
	RELAY_2 = 1;
	
	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ3�룬�ô󲿷�Ԫ���ȶ�����
	delayMS( 3000 );
	
	// initialize AM2301
	initAM2301();
	
	//��ʼ���ж�0
	initINT0();
		
	//initialize 24L01
	nrf24L01Init();
	//����24L01Ϊ����ģʽPRX
	nrfSetRxMode();
	
	//��ʼ����ʱ��0
	initTimer0();
	
	//SENSOR = 0;
	
	// ��������������
	powerMeterCount.longVal = 0;
	
	while(1)
	{
//		sendData(); 
//		delayMS(60000);
		
//		thisSensor = SENSOR;
//		if( lastSensor!= thisSensor && thisSensor ==1 )
//		{
//			sendPIRData();
//			
//			//����24L01Ϊ����ģʽPRX
//			nrfSetRxMode();
//		}
//		lastSensor = thisSensor;

		thisPowerMeter = POWER_METER;
		if( thisPowerMeter != lastPowerMeter )
		{
			if( thisPowerMeter == 0 ) //�͵�ƽ����
			{
				powerMeterCount.longVal++;
			}
			lastPowerMeter = thisPowerMeter;
		}
		
		if( sendDataFlag )
		{
			//���ʶλ
			sendDataFlag = 0;
			
			//������ʪ�����ݸ�����
			sendAM2301Data();
			
			//����24L01Ϊ����ģʽPRX
			nrfSetRxMode();
		}
				
		
	}
}

//��ʱ��0�жϴ������
void interruptTimer0(void) interrupt 1
{
	TR0=0; //�رն�ʱ��0
	ET0=0; //�رն�ʱ��0�ж�
	//TH0=0x8f; //����ʱ��0�ظ���ֵ����ʱʱ��50ms���ڲ�RC��
	//TL0=0x00; //��װ��8λ���������ֵ�� 
	//1f����22����24����30����40�죬36�죬32�죬31����32����36����40��
	
	TH0 = 0x77; //0x76
    TL0 = 0x48;	//0x48
	

	//TR0=1; //�򿪶�ʱ��0	

	if( ++timerMSCounter==200 ) //ÿ200���ж���1�룬����5ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter == sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //�򿪶�ʱ��0�ж�
	TR0=1; //�򿪶�ʱ��0


}

//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	receivedData = nrfGetReceivedData();
		
	RELAY_1 = *(receivedData++)==0?1:0;
	RELAY_2 = *(receivedData++)==0?1:0;
}