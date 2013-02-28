// STC12C5608AD 1T单片机  4M外部晶体振荡

#include<reg52.h>
//#include <intrins.h>

#include "nrf24L01Node.h"
#include "adc.h"
#include "dht11.h"
#include "ds18B20.h"


//中断计数器，每50ms加一
unsigned char timerMSCounter = 0;

//中断计数器，每1秒加一
unsigned int timerSecCounter = 0;

//自主发送数据间隔
unsigned int sendDataInterval = 60;
bit sendDataFlag = 1;


//void initINT0(void)
//{
//	EA=1;
//	EX0=1; // Enable int0 interrupt.
//}

void initTimer0(void)
{
	TMOD &= 0xf0; //清除定时器0模式位
	TMOD |= 0x01; //设定定时器0为方式1：16位计数器


	//TH0=0x8f; //给定时器赋初值，定时时间50ms，内部RC振荡
	//TL0=0x00; 
	
	TH0 = 0x0Bf;  //给定时器赋初值，定时时间50ms，1T STC12C5608AD 4M晶振
    TL0 = 0x085;


	ET0=1; //允许定时 0 中断 
	EA=1; //允许总中断 
	TR0=1; //启动定时器 0 
	
	//清计数器
	timerMSCounter = 0;
	timerSecCounter = 0;
	

}

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M晶振
}


////通过24L01将AM2301数据发送给主机
//void sendAM2301Data()
//{
//	unsigned char txData[HOST_DATA_WIDTH];
//	
//	txData[0] = NODE_ID_0; //Node ID0
//	txData[1] = NODE_ID_1; //Node ID1
//	
////	txData[2] = readAM2301(2);
////	txData[3] = getAM2301Data(0);
////	txData[4] = getAM2301Data(1);
////	txData[5] = getAM2301Data(2);
////	txData[6] = getAM2301Data(3);
////	txData[7] = getAM2301Data(4);
//		
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
//}

void sendData()
{
	unsigned char txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;
	
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	
	txData[2] = getADCResult(1);
	txData[3] = getHumidity(1);
	
	
	// 取得温度值
	temperature = getTemperature(1);
	// 四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	txData[4] = temperature<0 ? 1 : 0 ; //如果是零下则此位置1
	txData[5] = intTemperature/10; //温度的整数部分
	txData[6] = intTemperature%10; //温度的小数部分
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送

}

void main( void )
{

	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时3秒，让大部分元件稳定下来
	delayMS( 3000 );
	
	// initialize ADC
	initADC();
	
	// initialize AM2301
	//initAM2301();
	
	// initialize DHT11
	initDHT11();
	
	//initialize DS18B20
	initDS18B20();
	
	//initialize 24L01
	nrf24L01Init();
		
	//初始化定时器0
	initTimer0();
	
	
	while(1)
	{
		//sendData(); 
		//delayMS(3000);
		
		if( sendDataFlag )
		{
			//清标识位
			sendDataFlag = 0;
			
			//发送数据给主机
			sendData();
		}
				
		
	}
}

//定时器0中断处理程序
void interruptTimer0(void) interrupt 1
{
	TR0=0; //关闭定时器0
	ET0=0; //关闭定时器0中断

	TH0 = 0x0Bf; //给定时器0重赋初值，定时时间50ms 4M晶振 1T STC12C5608AD
    TL0 = 0x0a5; //重装低8位（修正后的值）

	
	TR0=1; //打开定时器0

	if( ++timerMSCounter==20 ) //每20次中断是1秒，即，50ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter == sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //打开定时器0中断
}
