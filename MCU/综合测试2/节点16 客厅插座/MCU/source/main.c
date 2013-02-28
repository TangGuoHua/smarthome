//STC12c5608ad 4M External Crystal

#include <reg52.h>
#include <intrins.h>
#include "nrf24L01Node.h"
//#include "adc.h"
//#include "ds18b20.h"
#include "am2301.h"

sbit RELAY_1 = P2^1;
sbit RELAY_2 = P2^0;

sfr AUXR   = 0x8E;

unsigned char cnt10ms = 0;
unsigned char second = 0;

//int intTemperature;
//bit showTime = 1;  //1: show time,  0: show temperature
//unsigned char showSecondCnt =0; //用来记时间/温度显示的秒数

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
	
	//启动timer0
    initTimer0();
	
	//初始化AM2301
	initAM2301();
	
	//初始化中断0
	initINT0();

	//初始化24L01
	nrf24L01Init();
	
	//设置24L01为接收模式PRX
	nrfSetRxMode();
	
	
	while(1)
	{

		
	}
}

void timer0Interrupt(void) interrupt 1
{
	unsigned char sendData[15];
//	float temperature;

    TH0 = 0x64; //64 每小时
    TL0 = 0x34; //30 每小时慢1秒
    
	TR0=0;
		
	if( ++cnt10ms==100 )
	{
		cnt10ms=0;
		second++;
		
		if( second == 10 )
		{
			second = 0;
			// 发送数据
			// initialize Node ID
			sendData[0]=NODE_ID_0;
			sendData[1]=NODE_ID_1;

			sendData[2] = readAM2301(1);
			sendData[3] = getAM2301Data(0);
			sendData[4] = getAM2301Data(1);
			sendData[5] = getAM2301Data(2);
			sendData[6] = getAM2301Data(3);
			sendData[7] = getAM2301Data(4);
			nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
			//设置24L01为接收模式PRX
			nrfSetRxMode();
		}
		
//		RELAY_1 = ~RELAY_1;
//		RELAY_2 = ~RELAY_2;
		
//		if( showTime )
//		{
//			if( ++showSecondCnt == 6 )  //时间显示6秒
//			{
//				showTime = 0;
//				showSecondCnt=0;
//			}
//		}
//		else
//		{
//			if( ++showSecondCnt == 2 ) //温度显示2秒
//			{
//				brightness = getADCResult(2);  //测亮度
//			
//				temperature = readTemperature(); //测温度
//				intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
//				
//				showTime = 1;
//				showSecondCnt=0;
//			}
//		}
//			
//		
//		if( second%5 == 0 )
//		{
//			
//			if( second==60 )
//			{
//				second=0;
//				minute++;
//				
//				if( minute%5 == 0 )
//				{
//				
//					// 发送亮度数据
//					// initialize Node ID
//					sendData[0]=NODE_ID_0;
//					sendData[1]=NODE_ID_1;
//					sendData[2]=brightness;
//					
//					sendData[3]= temperature<0 ? 1 : 0; //如果是零下则此位置1
//					sendData[4]= intTemperature/10; //温度的整数部分
//					sendData[5]= intTemperature%10; //温度的小数部分
//							
//					nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
//					
//					//设置24L01为接收模式PRX
//					nrfSetRxMode();
//					
//					if( minute==60 )
//					{
//						minute=0;
//						hour++;
//						if( hour==24)
//						{
//							hour=0;
//						}
//					}
//				}
//	
//			}
//		}
	}
	
	TR0=1;
}

//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	receivedData = nrfGetReceivedData();
		
	RELAY_1 = *(receivedData++)==0?1:0;
	RELAY_2 = *(receivedData++)==0?1:0;
}