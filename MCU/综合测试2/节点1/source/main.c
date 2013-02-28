/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
2011-OCT-07 Changhao   ����interval����STCƬ��EEPROM�Ĳ��֣���֤�´ο�����֮ǰ�����ò��ᶪʧ
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "dht11.h"
#include "ds18B20.h"
#include "delay.h"
#include "stcEEPROM.h"


//�жϼ�������ÿ50ms��һ
byte timerMSCounter = 0;

//�жϼ�������ÿ1���һ
uint timerSecCounter = 0;

//�����������ݼ��
uint sendDataInterval = 10;
bit sendDataFlag = 0;


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
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

//����ʪ�����ݷ��͸�����
void sendHTData()
{
	byte txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;

	byte humidity;
	
	humidity = getHumidity(1);
	
	temperature = readTemperature(1);
	
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 

	txData[0] = 100; //Node ID0
	txData[1] = 1; //Node ID1
	txData[2] = humidity;
	txData[3] = temperature<0 ? 1 : 0; //������������λ��1
	txData[4] = intTemperature/10; //�¶ȵ���������
	txData[5] = intTemperature%10; //�¶ȵ�С������
	txData[6] = 0;
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����

}

void serialPortInit(void)
{

	//28800bps@11.0592MHz
	PCON &= 0x7f; //�����ʲ�����
	SCON = 0x50;  //8λ����,�ɱ䲨���� ��ʽ1
	REN = 0;      //���������
	TMOD &= 0x0f; //�����ʱ��1ģʽλ
	TMOD |= 0x20; //�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFF;   //�趨��ʱ��ֵ
	TH1 = 0xFF;   //�趨��ʱ����װֵ
	ET1 = 0;      //��ֹ��ʱ��1�ж�
	//EA = 1;       //�����ж�
	//ES = 1;       //�򿪴����ж�
	ES=0;          //�رմ����ж�
	TR1 = 1;      //������ʱ��1
}


/*-----------
����һ���ֽ�
-----------*/
void serialPortSendByte( byte dat )
{
	//ES=0; //�رմ����ж�
	SBUF = dat;
	while(!TI); //�ȴ��������
	TI = 0;  //�巢����ϱ�־
	//ES=1;  //���������ж�
}

//��ģʽ�Ŵ���Ƭ��eeprom
void saveMode( byte mode )
{
	eepromEraseSector( STC_EEPROM_START_ADDR );
	eepromWrite( STC_EEPROM_START_ADDR, mode );
}

//��Ƭ��eepromȡ��mode��
byte getMode()
{
	return eepromRead(STC_EEPROM_START_ADDR);
}


//����ģʽ�ŷ��ض�Ӧ�ķ�������ʱ�������룩
uint getIntervalByMode( byte mode )
{
	uint interval;
	
	switch( mode )
	{
		case 0:
			interval = 0; //�ڵ㲻�Զ�������ʪ��ֵ�����������Ͷ�ȡ�����ȡ
			break;
			
		case 1:
			interval = 5; //5sec
			break;

		case 2:
			interval = 10; //10sec
			break;

		case 3:
			interval = 15; //15sec
			break;
			
		case 4:
			interval = 20; //20sec
			break;

		case 5:
			interval = 30; //30sec
			break;

		case 6:
			interval = 45; //45sec
			break;
			
		case 7:
			interval = 60; //1min
			break;

		case 8:
			interval = 90; //1.5min
			break;
			
		case 9:
			interval = 120; //2min
			break;
			
		case 10:
			interval = 180; //3min
			break;

		case 11:
			interval = 300; //5min
			break;
			
		case 12:
			interval = 600; //10min
			break;

		case 13:
			interval = 900; //15min
			break;

		case 14:
			interval = 1200; //20min
			break;

		case 15:
			interval = 1800; //30min
			break;

		case 16:
			interval = 2700; //45min
			break;

		case 17:
			interval = 3600; //1hour
			break;

		case 18:
			interval = 5400; //1.5hours
			break;

		case 19:
			interval = 7200; //2hours
			break;

		case 20:
			interval = 10800; //3hours
			break;

		case 21:
			interval = 18000; //5hours
			break;

		case 22:
			interval = 21600; //6hours
			break;

		case 23:
			interval = 28800; //8hours
			break;

		case 24:
			interval = 36000; //10hours
			break;

		case 25:
			interval = 43200; //12hours
			break;

		case 26:
			interval = 57600; //16hours
			break;

		case 27:
			interval = 64800; //18hours
			break;
		
		//ע�⣺sendDataInterval���޷������ͱ��������ֵ65535

		default:
			interval = 300; // Ĭ��5���Ӽ��
			break;

	} //swtich ����
	return interval;
}


void setNewInterval( byte mode )
{
	//���µ�ǰ��interval��������������ж��Ƿ��޸ĳɹ�
	uint oldInterval = sendDataInterval;
	
	//�õ��µ�interval
	sendDataInterval = getIntervalByMode( mode );
	
	if( sendDataInterval != oldInterval ) //˵��interval�仯��
	{

		//ֻҪinterval�仯�ˣ���Ҫ�����䵽eeprom�������´ο�����ָ�Ĭ��ֵ
		saveMode( mode );
		
		//����µ�interval������0�������³�ʼ����ʱ��0
		if( sendDataInterval !=0 )
		{
			//��ʼ����ʱ��0
			//���°��µ�interval��ʼ����
			initTimer0();
		}
		else
		{
			//intervalΪ0��ζ�Ų����������ͣ�������ѯ
			TR0=0; //�رն�ʱ��0
		}
	}
}


//������
void main()
{
	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ3�룬�ô󲿷�Ԫ���ȶ�����
	//delayMS( 2000 );
	
	//serialPortInit();
	
	//��ʼ���ж�0
	initINT0();

	//��ʼ��DHT11
	initDHT11();
	
	//��ʼ��24L01
	nrf24L01Init();
	
	//����24L01Ϊ����ģʽPRX
	nrfSetRxMode();
	
	//��ʼ����ʱ��0
	initTimer0();
	
	//��ʼ��Interval
	sendDataInterval = getIntervalByMode( getMode() );
	
	
	while(1)
	{
		if( sendDataFlag )
		{
			//���ʶλ
			sendDataFlag = 0;
			
			//������ʪ�����ݸ�����
			sendHTData();
			
			//����24L01Ϊ����ģʽPRX
			nrfSetRxMode();
		}


	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	byte* receivedData;
	byte cmd, param;
	
	TR0=0; //�رն�ʱ��0
	
	receivedData = nrfGetReceivedData();
	cmd = *receivedData++ ;
	param = *receivedData++;
	
	serialPortSendByte( cmd );
	serialPortSendByte( param );
	
	switch(cmd)
	{
		case 80:
			//������ʪ������
			sendHTData();
			
			//���ʱ��
			//�´η��ؾ��Ǵ����ڿ�ʼ��ʱ�ģ�sendDataInterval+̽ͷ����ʱ�䣩֮��
			timerSecCounter = 0;
			break;
		
		case 90: //�����µ�interval
			setNewInterval( param );
			break;
		
		default:
			break;	
	}
	
	//�ٴν������ģʽ
	nrfSetRxMode();
	
	TR0=1; //�򿪶�ʱ��0
}

//��ʱ��0�жϴ������
void interruptTimer0(void) interrupt 1
{
	TR0=0; //�رն�ʱ��0
	ET0=0; //�رն�ʱ��0�ж�
	TH0=0x4c; //����ʱ��0�ظ���ֵ����ʱʱ��50ms��11.0592MHz����
	TL0=0x35; //��װ��8λ���������ֵ�� 
	//1f����22����24����30����40�죬36�죬32�죬31����32����36����40��
	
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
