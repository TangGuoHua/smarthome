/*-----------------------------------------------
  名称：DS18b20 温度检测液晶显示
  论坛：www.doflye.net
  编写：shifang
  日期：2009.5
  修改：无
  内容：
------------------------------------------------*/
#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义

#include "ds18b20.h"
#include "delay.h"
//#include "com.h"
#include "dht11.h"
#include "adc.h"
#include "nrf24L01Node.h"
#include "am2301.h"

#define LDR_ADC_CH 3  // 光敏电阻在ADC的哪个通道上？

sbit DOOR_OPEN  = P1^2 ;


//中断计数器，每50ms加一
unsigned char timerMSCounter = 0;

//中断计数器，每1秒加一
unsigned int timerSecCounter = 0;

//自主发送数据间隔 (秒)
unsigned int sendDataInterval = 300;

bit sendDataFlag = 1;


//取得亮度值采样
unsigned char getBrightness()
{
	unsigned char i;
	unsigned int sum=0;
	
	//采样N次 取平均值
	for(i=0; i<200; i++)
		sum += getADCResult( LDR_ADC_CH );
		
	return (sum/200);
}

void initTimer0(void)
{
	TMOD &= 0xf0; //清除定时器0模式位
	TMOD |= 0x01; //设定定时器0为方式1：16位计数器


	TH0=0x4c; //给定时器赋初值，定时时间50ms，11.0592MHz晶振
	TL0=0x40; 

	ET0=1; //允许定时 0 中断 
	EA=1; //允许总中断 
	TR0=1; //启动定时器 0 
	
	//清计数器
	timerMSCounter = 0;
	timerSecCounter = 0;
}



//通过24L01将数据发送给主机
void sendHTDataNRF24L01()
{
	
	byte txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;
	
	// DS18B20取得温度值
	temperature = readTemperature(1);
	
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	
	txData[2] = getBrightness(); //亮度
	
	
	txData[3] = readAM2301(2);
	txData[4] = getAM2301Data(0);
	txData[5] = getAM2301Data(1);
	txData[6] = getAM2301Data(2);
	txData[7] = getAM2301Data(3);
	txData[8] = getAM2301Data(4);
	
	txData[9] = getHumidity(1);
	
	txData[10]= temperature<0 ? 1 : 0; //如果是零下则此位置1
	txData[11]= intTemperature/10; //温度的整数部分
	txData[12]= intTemperature%10; //温度的小数部分
	
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
}


//通过24L01将Door open 数据发送给主机
void sendDoorOpenDataNRF24L01( bit doorOpen)
{
	
	byte txData[HOST_DATA_WIDTH];
	
	// 门打开否 使用100 55这个节点发送
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = 55; //Node ID1
	
	txData[2] = doorOpen;
	
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
}

void main (void)
{
	static bit lastDoorOpen=0;
	
	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时2秒，让大部分元件稳定下来
	delayMS( 2000 );
	
	//初始化串口
	//serialPortInit();
	
	//初始化ADC
	initADC();
	
	//初始化DS18B20
	initDS18B20();
	
	//初始化DHT11
	initDHT11();
	
	//初始化AM2301	
	initAM2301();
	
	//初始化24L01+
	nrf24L01Init();
	
	//初始化定时器0
	initTimer0();
	
	while(1)
	{
		if( sendDataFlag )
		{
			//清标识位
			sendDataFlag = 0;
			
			//发送数据给主机
			sendHTDataNRF24L01();
		}
		
		//检测门是否打开
		if( lastDoorOpen != DOOR_OPEN )
		{
			delayMS( 1000 ); //去干扰
			if( lastDoorOpen != DOOR_OPEN )
			{
				lastDoorOpen = DOOR_OPEN;
				sendDoorOpenDataNRF24L01( lastDoorOpen );
				//delayMS( 800 ); //800ms延时 防抖动
			}
		}
	}
}



//定时器0中断处理程序
void interruptTimer0(void) interrupt 1
{
	TR0=0; //关闭定时器0
	ET0=0; //关闭定时器0中断
	TH0=0x4c; //给定时器0重赋初值，定时时间50ms，11.0592MHz晶振
	TL0=0x40; //重装低8位（修正后的值） 
	//1f慢，22慢，24慢，30慢，40快，36快，32快，28快, 24快，20快，1a快，00稍慢
	// 2011.11.30 TH0=0x4c TL0=0x00 准了 sendDataInterval=10s 11.0592M STC12C5608AD
	
	TR0=1; //打开定时器0

	if( ++timerMSCounter>19 ) //每20次中断是1秒，即，50ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter >= sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //打开定时器0中断
}
