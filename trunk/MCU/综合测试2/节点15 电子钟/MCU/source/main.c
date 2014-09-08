//STC12c5608ad 4M External Crystal

#include <reg52.h>
#include <intrins.h>
#include "nrf24L01Node.h"
#include "adc.h"
#include "ds18b20.h"

sbit LED_A = P2^1;
sbit LED_B = P1^0;
sbit LED_C = P1^4;
sbit LED_D = P1^6;
sbit LED_E = P1^7;
sbit LED_F = P2^0;
sbit LED_G = P1^3;
sbit LED_P = P1^5;

sbit DIG_1 = P2^2;
sbit DIG_2 = P2^7;
sbit DIG_3 = P3^7;
sbit DIG_4 = P1^1;

sfr AUXR   = 0x8E;

unsigned char cnt10ms = 0;
unsigned char second=0, minute=0, hour=0;
unsigned char brightness=0;

int intTemperature;
bit showTime = 1;  //1: show time,  0: show temperature
unsigned char showSecondCnt =0; //������ʱ��/�¶���ʾ������


void delayLedOn(void)
{
    unsigned char a,b;
    for(b=5;b>0;b--)
        for(a=80;a>0;a--);
}

void delayLedOff(void)   //��� 0us
{
    unsigned char a,b;
	
	if( brightness < 100 )
	{
	    for(b=100-brightness ;b>0;b--)
	        for(a=40;a>0;a--);
	}
}

// dig - ��ʾ����ܵڼ�λ, �������ҷֱ��� 1-4
// num - Ҫ��ʾ������, 0-9
// showPoint - 1��ʾС���㣬0����ʾС����
void display( unsigned char dig, unsigned char num, bit showPoint )
{
	switch( dig )
	{
		case 1:
			DIG_1 = 0;
			DIG_2 = 1;
			DIG_3 = 1;
			DIG_4 = 1;
			break;
		case 2:
			DIG_1 = 1;
			DIG_2 = 0;
			DIG_3 = 1;
			DIG_4 = 1;
			break;
		case 3:
			DIG_1 = 1;
			DIG_2 = 1;
			DIG_3 = 0;
			DIG_4 = 1;
			break;
		case 4:
			DIG_1 = 1;
			DIG_2 = 1;
			DIG_3 = 1;
			DIG_4 = 0;
			break;
	}
	
	LED_P = showPoint?0:1;
	
	
									
	switch( num )
	{
		case 1:
			LED_A=1;
			LED_B=0;
			LED_C=0;
			LED_D=1;
			LED_E=1;
			LED_F=1;
			LED_G=1;
			break;
		case 2:
			LED_A=0;
			LED_B=0;
			LED_C=1;
			LED_D=0;
			LED_E=0;
			LED_F=1;
			LED_G=0;
			break;
		case 3:
			LED_A=0;
			LED_B=0;
			LED_C=0;
			LED_D=0;
			LED_E=1;
			LED_F=1;
			LED_G=0;
			break;
		case 4:
			LED_A=1;
			LED_B=0;
			LED_C=0;
			LED_D=1;
			LED_E=1;
			LED_F=0;
			LED_G=0;
			break;
		case 5:
			LED_A=0;
			LED_B=1;
			LED_C=0;
			LED_D=0;
			LED_E=1;
			LED_F=0;
			LED_G=0;
			break;
		case 6:
			LED_A=0;
			LED_B=1;
			LED_C=0;
			LED_D=0;
			LED_E=0;
			LED_F=0;
			LED_G=0;
			break;
		case 7:
			LED_A=0;
			LED_B=0;
			LED_C=0;
			LED_D=1;
			LED_E=1;
			LED_F=1;
			LED_G=1;
			break;
		case 8:
			LED_A=0;
			LED_B=0;
			LED_C=0;
			LED_D=0;
			LED_E=0;
			LED_F=0;
			LED_G=0;
			break;
		case 9:
			LED_A=0;
			LED_B=0;
			LED_C=0;
			LED_D=0;
			LED_E=1;
			LED_F=0;
			LED_G=0;
			break;
		case 0:
			LED_A=0;
			LED_B=0;
			LED_C=0;
			LED_D=0;
			LED_E=0;
			LED_F=0;
			LED_G=1;
			break;
		case ' ':
		default:
			LED_A=1;
			LED_B=1;
			LED_C=1;
			LED_D=1;
			LED_E=1;
			LED_F=1;
			LED_G=1;
			break;																	
	}
	delayLedOn();

	LED_A=1;
	LED_B=1;
	LED_C=1;
	LED_D=1;
	LED_E=1;
	LED_F=1;
	LED_G=1;
	LED_P=1;
	
	delayLedOff();

	
}


void initTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0x0C0;
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

	unsigned char bri =0;
		
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//��ʼ��ADC
	initADC();
	
    initTimer0();
	
	//��ʼ���ж�0
	initINT0();

	//��ʼ��24L01
	nrf24L01Init();
	
	//����24L01Ϊ����ģʽPRX
	nrfSetRxMode();
	
	DIG_1 = 1;
	DIG_2 = 1;
	DIG_3 = 1;
	DIG_4 = 1;
	
	while(1)
	{
		if( showTime )
		{
			display(1, hour/10, 0);
			display(2, hour%10, 1);
			display(3, minute/10, 0);
			display(4, minute%10, 0);
		}
		else
		{
			display(1, 100, 0); //��ʾ��
			display(2, intTemperature/100, 0 );
			display(3, intTemperature%100/10, 1);
			display(4, intTemperature%100%10, 0);
		}
			
//		bri = getADCResult(2);  //������
//		brightness = bri;
//		display(1, ' ', 0); //��ʾ��
//		display(2, bri/100, 0 );
//		display(3, bri%100/10, 0);
//		display(4, bri%100%10, 0);		
		

		
	}
}

void timer0Interrupt(void) interrupt 1
{
	unsigned char sendData[15];
	float temperature;

    TH0 = 0x64; //64 ÿСʱ
    TL0 = 0x34; //30 ÿСʱ��1��
    
	TR0=0;
		
	if( ++cnt10ms==100 )
	{
		cnt10ms=0;
		second++;
		
		if( showTime )
		{
			if( ++showSecondCnt == 6 )  //ʱ����ʾ6��
			{
				showTime = 0;
				showSecondCnt=0;
			}
		}
		else
		{
			if( ++showSecondCnt == 2 ) //�¶���ʾ2��
			{
				brightness = getADCResult(2);  //������
			
				temperature = readTemperature(); //���¶�
				intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
				
				showTime = 1;
				showSecondCnt=0;
			}
		}
			

		
		if( second%5 == 0 )
		{
			
			if( second==60 )
			{
				second=0;
				minute++;
				
				if( minute%1 == 0 )
				{
				
					// ������������
					// initialize Node ID
					sendData[0]=NODE_ID_0;
					sendData[1]=NODE_ID_1;
					sendData[2]=brightness;
					
					sendData[3]= temperature<0 ? 1 : 0; //������������λ��1
					sendData[4]= intTemperature/10; //�¶ȵ���������
					sendData[5]= intTemperature%10; //�¶ȵ�С������
							
					nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
					
					//����24L01Ϊ����ģʽPRX
					nrfSetRxMode();
					
					if( minute==60 )
					{
						minute=0;
						hour++;
						if( hour==24)
						{
							hour=0;
						}
					}
				}
	
			}
		}
	}
	
	TR0=1;
}

//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;

	receivedData = nrfGetReceivedData();
	
	hour = *(receivedData++);
	minute = *(receivedData++);
	second = *(receivedData++);
}