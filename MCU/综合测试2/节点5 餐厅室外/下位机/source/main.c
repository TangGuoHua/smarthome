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
//#include "com.h"
#include "dht11.h"
#include "adc.h"
#include "nrf24L01Node.h"
#include "am2301.h"

#define LDR_ADC_CH 3  // ����������ADC���ĸ�ͨ���ϣ�

sbit DOOR_OPEN  = P1^2 ;


//�жϼ�������ÿ50ms��һ
unsigned char timerMSCounter = 0;

//�жϼ�������ÿ1���һ
unsigned int timerSecCounter = 0;

//�����������ݼ�� (��)
unsigned int sendDataInterval = 300;

bit sendDataFlag = 1;


//ȡ������ֵ����
unsigned char getBrightness()
{
	unsigned char i;
	unsigned int sum=0;
	
	//����N�� ȡƽ��ֵ
	for(i=0; i<200; i++)
		sum += getADCResult( LDR_ADC_CH );
		
	return (sum/200);
}

void initTimer0(void)
{
	TMOD &= 0xf0; //�����ʱ��0ģʽλ
	TMOD |= 0x01; //�趨��ʱ��0Ϊ��ʽ1��16λ������


	TH0=0x4c; //����ʱ������ֵ����ʱʱ��50ms��11.0592MHz����
	TL0=0x40; 

	ET0=1; //����ʱ 0 �ж� 
	EA=1; //�������ж� 
	TR0=1; //������ʱ�� 0 
	
	//�������
	timerMSCounter = 0;
	timerSecCounter = 0;
}



//ͨ��24L01�����ݷ��͸�����
void sendHTDataNRF24L01()
{
	
	byte txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;
	
	// DS18B20ȡ���¶�ֵ
	temperature = readTemperature(1);
	
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	
	txData[2] = getBrightness(); //����
	
	
	txData[3] = readAM2301(2);
	txData[4] = getAM2301Data(0);
	txData[5] = getAM2301Data(1);
	txData[6] = getAM2301Data(2);
	txData[7] = getAM2301Data(3);
	txData[8] = getAM2301Data(4);
	
	txData[9] = getHumidity(1);
	
	txData[10]= temperature<0 ? 1 : 0; //������������λ��1
	txData[11]= intTemperature/10; //�¶ȵ���������
	txData[12]= intTemperature%10; //�¶ȵ�С������
	
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
}


//ͨ��24L01��Door open ���ݷ��͸�����
void sendDoorOpenDataNRF24L01( bit doorOpen)
{
	
	byte txData[HOST_DATA_WIDTH];
	
	// �Ŵ򿪷� ʹ��100 55����ڵ㷢��
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = 55; //Node ID1
	
	txData[2] = doorOpen;
	
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
}

void main (void)
{
	static bit lastDoorOpen=0;
	
	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ2�룬�ô󲿷�Ԫ���ȶ�����
	delayMS( 2000 );
	
	//��ʼ������
	//serialPortInit();
	
	//��ʼ��ADC
	initADC();
	
	//��ʼ��DS18B20
	initDS18B20();
	
	//��ʼ��DHT11
	initDHT11();
	
	//��ʼ��AM2301	
	initAM2301();
	
	//��ʼ��24L01+
	nrf24L01Init();
	
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
		}
		
		//������Ƿ��
		if( lastDoorOpen != DOOR_OPEN )
		{
			delayMS( 1000 ); //ȥ����
			if( lastDoorOpen != DOOR_OPEN )
			{
				lastDoorOpen = DOOR_OPEN;
				sendDoorOpenDataNRF24L01( lastDoorOpen );
				//delayMS( 800 ); //800ms��ʱ ������
			}
		}
	}
}



//��ʱ��0�жϴ������
void interruptTimer0(void) interrupt 1
{
	TR0=0; //�رն�ʱ��0
	ET0=0; //�رն�ʱ��0�ж�
	TH0=0x4c; //����ʱ��0�ظ���ֵ����ʱʱ��50ms��11.0592MHz����
	TL0=0x40; //��װ��8λ���������ֵ�� 
	//1f����22����24����30����40�죬36�죬32�죬28��, 24�죬20�죬1a�죬00����
	// 2011.11.30 TH0=0x4c TL0=0x00 ׼�� sendDataInterval=10s 11.0592M STC12C5608AD
	
	TR0=1; //�򿪶�ʱ��0

	if( ++timerMSCounter>19 ) //ÿ20���ж���1�룬����50ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter >= sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //�򿪶�ʱ��0�ж�
}
