/*-----------------------------------------------
  ���ƣ�DS18b20 �¶ȼ��Һ����ʾ
  ��̳��www.doflye.net
  ��д��shifang
  ���ڣ�2009.5
  �޸ģ���
  ���ݣ�
------------------------------------------------*/
#include<reg52.h> //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���

#include "ds18b20.h"
#include "delay.h"
#include "com.h"
#include "dht11.h"
#include "adc.h"
#include "nrf24L01Node.h"
#include "am2301.h"

#define LDR_ADC_CH 0  // ����������ADC���ĸ�ͨ���ϣ�


//�жϼ�������ÿ50ms��һ
unsigned char timerMSCounter = 0;

//�жϼ�������ÿ1���һ
unsigned int timerSecCounter = 0;

//�����������ݼ�� (��)
unsigned int sendDataInterval = 10;

bit sendDataFlag = 1;


//ȡ������ֵ����
unsigned char getBrightness()
{
	unsigned char i;
	unsigned int sum=0;
	
	//����100�� ȡƽ��ֵ
	for(i=0; i<100; i++)
		sum += getADCResult( LDR_ADC_CH );
		
	return (sum/100);
}

void initTimer0(void)
{
	TMOD &= 0xf0; //�����ʱ��0ģʽλ
	TMOD |= 0x01; //�趨��ʱ��0Ϊ��ʽ1��16λ������


	TH0=0x4c; //����ʱ������ֵ����ʱʱ��50ms��11.0592MHz����
	TL0=0x00; 

	ET0=1; //����ʱ 0 �ж� 
	EA=1; //�������ж� 
	TR0=1; //������ʱ�� 0 
	
	//�������
	timerMSCounter = 0;
	timerSecCounter = 0;
}

//ͨ�����ڽ����ݷ��͸�����
void sendHTDataSerialPort()
{
	float temperature;
	int intTemperature;

	//֡ͷ
	serialPortSendByte( 0xba );
	serialPortSendByte( 0xba );
	serialPortSendByte( 0x98 );
	serialPortSendByte( 0x76 );
	
	//��������ֵ
	serialPortSendByte( getBrightness() );

	// ����ʪ��ֵ
	serialPortSendByte( getHumidity(1));
	
	// ȡ���¶�ֵ
	temperature = readTemperature(1);
	
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 


	serialPortSendByte( temperature<0 ? 1 : 0 ); //������������λ��1
	serialPortSendByte( intTemperature/10); //�¶ȵ���������
	serialPortSendByte( intTemperature%10); //�¶ȵ�С������
	
	serialPortSendByte( 0x88 );
	serialPortSendByte( readAM2301() );
	serialPortSendByte( getAM2301Data(0));
	serialPortSendByte( getAM2301Data(1));
	serialPortSendByte( getAM2301Data(2));
	serialPortSendByte( getAM2301Data(3));
	serialPortSendByte( getAM2301Data(4));
	
	//֡β
	serialPortSendByte( 0x54 );
	serialPortSendByte( 0x32 );
	serialPortSendByte( 0x10 );	
}

//ͨ��24L01�����ݷ��͸�����
void sendHTDataNRF24L01()
{
	
	byte txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;

	byte humidity;
	byte brightness;
	
	//ʪ��
	humidity = getHumidity(1);
	
	//�¶�
	temperature = readTemperature(1);
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	//����
	brightness = getBrightness();
	
	

	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	txData[2] = humidity;
	txData[3] = temperature<0 ? 1 : 0; //������������λ��1
	txData[4] = intTemperature/10; //�¶ȵ���������
	txData[5] = intTemperature%10; //�¶ȵ�С������
	txData[6] = brightness; //����
	txData[7] = 88;
	txData[8] = readAM2301();
	txData[9] = getAM2301Data(0);
	txData[10] = getAM2301Data(1);
	txData[11] = getAM2301Data(2);
	txData[12] = getAM2301Data(3);
	txData[13] = getAM2301Data(4);
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����

}


void main (void)
{
	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ3�룬�ô󲿷�Ԫ���ȶ�����
	delayMS( 3000 );
	
	//��ʼ������
	//serialPortInit();
	
	//��ʼ��ADC
	initADC();
	
	//��ʼ��DHT11
	initDHT11();
	
	//��ʼ����ʱ��0
	initTimer0();
	
	
	while(1)
	{
		if( sendDataFlag )
		{
			//���ʶλ
			sendDataFlag = 0;
			
			//�������ݸ�����
			sendHTDataNRF24L01();
			
			//sendHTDataSerialPort();
		}
	}
}



/*-------------
��ʱ���ж��ӳ���
--------------*/
//��ʱ��0�жϴ������
void interruptTimer0(void) interrupt 1
{
	TR0=0; //�رն�ʱ��0
	ET0=0; //�رն�ʱ��0�ж�
	TH0=0x4c; //����ʱ��0�ظ���ֵ����ʱʱ��50ms��11.0592MHz����
	TL0=0x33; //��װ��8λ���������ֵ�� 
	//1f����22����24����30����40�죬36�죬32�죬28��, 24�죬20�죬1a�죬00����
	// 2011.11.30 TH0=0x4c TL0=0x00 ׼�� sendDataInterval=10s 11.0592M STC12C5608AD
	
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
