/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC11F04E
����Ƶ��: 4MHz �ⲿ����
������
DS18B20�¶ȴ�����
30A�̵���ģ��

���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��02��05��  �Ƴ���  ��ʼ�汾
2014��10��03��  �Ƴ���  ����С�����Զ�����ģʽ
                        ����EEPROM��������
						��Pi�������ݵ���ʱ��ԭ10s��ʱ������Ϊtimer0�жϼ���
2014��12��28��  �Ƴ���  ���ӿ���������������¯�����Ʋ���

--------------
!! TODO: 
������ģ���շ�����ʱҪֹͣtimer���������ж����ʱ����� ���ο�91�Žڵ�ĳ���


����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

//STC11F04E 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"
#include "ds18B20.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 92

sfr AUXR   = 0x8E;

sbit RELAY_WATERHEATER = P3^7;  //С������ˮ�����Ƽ̵���
sbit RELAY_AIRHEATER1 = P1^4;
sbit RELAY_AIRHEATER2 = P1^5;

volatile unsigned char gWaterHeaterMode = 0; //0���أ�1������2���Զ����ܵ���PIR���ƣ�
volatile unsigned char gAirHeaterMode = 0; //0���أ�1������2���¿�
volatile unsigned char gSetTemperatureX10 = 0; //�趨���¶� (x10)����air heater����ģʽ=2ʱ��Ҫ��ﵽ���¶�ֵ
volatile int gCurrentTemperatureX10=0; //��ǰ�¶� (x10)

// Flag for sending data to Pi
volatile unsigned char gSendDataFunctNum = 0;
volatile unsigned char gTimerCounter10ms = 0;
volatile unsigned int gTimerCounter1s = 0;

//С������ˮ��ʱ
volatile bit gWaterHeaterDelayOn = 0; //С������ˮ��ʱ��ʼ��ֹͣ
volatile int gWaterHeaterDelayCounter1s = 0; 
volatile unsigned char gWaterHeaterDelayMinute = 0; //С������ˮ��ʱ������

volatile bit gPeopleIn = 0; //��������


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
	//50��
    unsigned n;
    for(n=0;n<10;n++)
	{
		delay5s();
	}

}


//��ʱ2��
void delay2s(void)
{
    unsigned char a,b,c;
    for(c=252;c>0;c--)
        for(b=230;b>0;b--)
            for(a=33;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


// ��ʼ��Timer0
// ÿ10ms�����ж�1һ��
// ��STC 1T @4MHz �ⲿ����
void initTimer0(void)
{
	
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}


//��ʼ���̵���
void initRelays()
{
	//ȫ��
	RELAY_WATERHEATER=1;
	RELAY_AIRHEATER1 = 1;
	RELAY_AIRHEATER2 = 1;	
}


//�򿪹رռ�����
//switchOn=0:��
//switchOn=1:��
void switchAirHeater( bit switchOn )
{
	if( switchOn )
	{
		RELAY_AIRHEATER1 = 0;
		RELAY_AIRHEATER2 = 0;
	}
	else
	{
		RELAY_AIRHEATER1 = 1;
		RELAY_AIRHEATER2 = 1;	
	}
}


//�������ò�����Ƭ��eeprom
void saveSettings()
{
	eepromEraseSector( 0x0000 );
	
	//����С������ˮ��ʱ������
	eepromWrite( 0x0000, gWaterHeaterDelayMinute );
	
	//�����趨�¶�
	eepromWrite( 0x0002, gSetTemperatureX10 );	
}


int getTemperatureX10()
{
	float temperature;
	int intTemperature;
	
	// DS18B20ȡ���¶�ֵ
	temperature = readTemperature(0);
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
	return (intTemperature*(temperature<0 ? -1 : 1));
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 92, 231, 92}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr);
}


//���������͵�ǰ���¶Ⱥͼ̵���״̬
void sendDataToHost( unsigned char funcNum )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;

	sendData[0]= NODE_ID;//Node ID
	sendData[1]= funcNum; //function number
	
	
	sendData[2]= gCurrentTemperatureX10<0 ? 1 : 0; //������������λ��1
	sendData[3]= ( gCurrentTemperatureX10*(gCurrentTemperatureX10<0 ? -1 : 1) )/10; //�¶ȵ���������
	sendData[4]= ( gCurrentTemperatureX10*(gCurrentTemperatureX10<0 ? -1 : 1) )%10; //�¶ȵ�С������
	
	sendData[5]= gWaterHeaterMode;
	sendData[6]= gWaterHeaterDelayMinute;
	sendData[7]= (~RELAY_WATERHEATER); //Water heater relay status
	
	sendData[8]= gAirHeaterMode;
	sendData[9]= gSetTemperatureX10;
	sendData[10]= (~RELAY_AIRHEATER1);
	sendData[11]= (~RELAY_AIRHEATER2);
	
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96Ƶ����5�ֽڵ�ַ������16�ֽ�
	
	//24L01��ʼ��������
	startRecv(); 
}



void main()
{

	//����T0Ϊ1Tģʽ
	AUXR = AUXR|0x80;  
	
	//��ʼ���ж�0
	initINT0();
	
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��DS18B20
	readTemperature(0);
	
	//ȡ��С������ˮ��ʱ������
	gWaterHeaterDelayMinute = eepromRead(0x0000);
	//ȡ���趨���¶�
	gSetTemperatureX10 = eepromRead(0x0002);
	
	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ����ʱ
	initDelay();
	
	// ��ʼ��Timer0
	initTimer0();
			
	while(1)
	{
		//ÿȦ��ʱ2�룬�Ա���ds18b20����
		delay2s();
		
		//����
		gCurrentTemperatureX10 = getTemperatureX10();
		
		//�������������¿�ģʽ��
		if( gAirHeaterMode == 2 )
		{
			if( gCurrentTemperatureX10 >= gSetTemperatureX10 + 1 )
			{
				switchAirHeater(0);
			}
			else if( gCurrentTemperatureX10 <= gSetTemperatureX10 - 1 )
			{
				switchAirHeater(1);
			}
		}
		
		//��������������
		if( gSendDataFunctNum !=0  )
		{
			sendDataToHost( gSendDataFunctNum );
			gSendDataFunctNum = 0;
			gTimerCounter1s = 0; //���¿�ʼ��ʱ
		}		
	}
}



//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char fromNodeID, funcNum;
	unsigned char tmp;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	fromNodeID = *receivedData;
	funcNum = *(receivedData+1);
	
	if( fromNodeID == 1 )//����Pi(#1)
	{
		if( funcNum == 20 || funcNum == 22)
		{
			//С��������ģʽ
			tmp = *(receivedData+2);
			
			if( gWaterHeaterMode != tmp ) //С��������ģʽ�仯��
			{
			
				//С��������
				if( tmp == 0 )//����
				{
					RELAY_WATERHEATER = 1;
					
					//���֮ǰС�������Զ�ģʽ����������ʱ��ˮ�����У�
					//��ֹͣ��ˮ��ʱ���������������
					gWaterHeaterDelayOn = 0; 
				}
				else if( tmp == 1 )//����
				{
					RELAY_WATERHEATER = 0;
					
					//���֮ǰС�������Զ�ģʽ����������ʱ��ˮ�����У�
					//��ֹͣ��ˮ��ʱ���������������
					gWaterHeaterDelayOn = 0; 
				}
				else if( tmp == 2 )//�Զ�
				{
					if( gPeopleIn )
					{
						//������ˣ���ʼ��ˮ
						gWaterHeaterDelayOn = 0; //��ˮ��ʱ��ʱֹͣ
						gWaterHeaterDelayCounter1s = 0;
						
						RELAY_WATERHEATER = 0; //��ʼ��ˮ			
					}
					else
					{
						//���û�ˣ���ֹͣ��ˮ
						RELAY_WATERHEATER = 1;
						gWaterHeaterDelayOn = 0;
					}
				}
				gWaterHeaterMode = tmp;
			}
			
			
			//��ˮ��ʱ������	
			tmp = *(receivedData+3);
			if( tmp != gWaterHeaterDelayMinute )//�������仯��
			{
				gWaterHeaterDelayMinute=tmp;
				saveSettings();
			}
			
			
			//�¶��趨
			//����ֵ��Χ 10-250����1-25��
			//
			tmp = *(receivedData+5);
			if( tmp != gSetTemperatureX10 )//�趨���¶ȱ仯��
			{
				gSetTemperatureX10=tmp;
				saveSettings();
			}
			
			
			//����������ģʽ	
			tmp = *(receivedData+4);			
			if( tmp != gAirHeaterMode )
			{
				if( tmp == 0 )
				{
					//�رռ�����
					switchAirHeater(0);
				}
				else if( tmp == 1 )
				{
					//�򿪼�����
					switchAirHeater(1);
				}
				else if( tmp == 2 )
				{
					//����Ϊ�¿�ģʽ
					if( gCurrentTemperatureX10 >= gSetTemperatureX10 + 1 )
					{
						switchAirHeater(0);
					}
					else if( gCurrentTemperatureX10 <= gSetTemperatureX10 - 1 )
					{
						switchAirHeater(1);
					}
				}
				
				gAirHeaterMode = tmp;

			}
		}
		
		if( funcNum == 22 )
		{
			gSendDataFunctNum = 23;
		}
		else if( funcNum == 50 )
		{
			gSendDataFunctNum = 51;
		}		
	}
	else if( fromNodeID == 91 ) //���Ե���������(#91)
	{
		if( funcNum == 2) //�¼�����
		{
			//��������
			gPeopleIn = (*(receivedData+3)==0)?0:1;
			
			//С�����������Զ�ģʽ��
			//��������Զ�ģʽ����������Ե���������PIR������
			if( gWaterHeaterMode == 2 ) 
			{
				if( gPeopleIn )
				{
					//�����ˣ���ˮ
									
					gWaterHeaterDelayOn = 0; //��ˮ��ʱ��ʱֹͣ
					gWaterHeaterDelayCounter1s = 0;
					
					RELAY_WATERHEATER = 0; //��ʼ��ˮ
				}
				else
				{
					//�����ˣ�������ˮ(waterHeaterDelayMinute)���ӣ�Ȼ���ٹ�С����
					gWaterHeaterDelayCounter1s = 0;
					gWaterHeaterDelayOn = 1; //��ˮ��ʱ��ʼ��ʱ
				}
			}
		}
	}
}



//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++gTimerCounter10ms == 100 ) //100��10ms����1��
	{
		gTimerCounter10ms=0;
		
		//��Pi��������
		if( ++gTimerCounter1s == 600 ) //ÿ600��һ��
		{
			gTimerCounter1s = 0;
			gSendDataFunctNum = 1; //��������
		}
		
		//С������ʱ
		if( gWaterHeaterDelayOn )
		{
			if( ++gWaterHeaterDelayCounter1s >= gWaterHeaterDelayMinute*60 )
			{
				//��ʱ�ѵ�
				gWaterHeaterDelayOn = 0; //ֹͣ��ʱ��ʱ
				gWaterHeaterDelayCounter1s = 0; //����������
				RELAY_WATERHEATER = 1; //��С����
			}
		}
	}
}