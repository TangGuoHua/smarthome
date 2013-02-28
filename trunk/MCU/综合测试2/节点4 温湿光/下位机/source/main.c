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
#include "com.h"
#include "dht11.h"
#include "adc.h"
#include "nrf24L01Node.h"
#include "am2301.h"

#define LDR_ADC_CH 0  // 光敏电阻在ADC的哪个通道上？


//中断计数器，每50ms加一
unsigned char timerMSCounter = 0;

//中断计数器，每1秒加一
unsigned int timerSecCounter = 0;

//自主发送数据间隔 (秒)
unsigned int sendDataInterval = 10;

bit sendDataFlag = 1;


//取得亮度值采样
unsigned char getBrightness()
{
	unsigned char i;
	unsigned int sum=0;
	
	//采样100次 取平均值
	for(i=0; i<100; i++)
		sum += getADCResult( LDR_ADC_CH );
		
	return (sum/100);
}

void initTimer0(void)
{
	TMOD &= 0xf0; //清除定时器0模式位
	TMOD |= 0x01; //设定定时器0为方式1：16位计数器


	TH0=0x4c; //给定时器赋初值，定时时间50ms，11.0592MHz晶振
	TL0=0x00; 

	ET0=1; //允许定时 0 中断 
	EA=1; //允许总中断 
	TR0=1; //启动定时器 0 
	
	//清计数器
	timerMSCounter = 0;
	timerSecCounter = 0;
}

//通过串口将数据发送给主机
void sendHTDataSerialPort()
{
	float temperature;
	int intTemperature;

	//帧头
	serialPortSendByte( 0xba );
	serialPortSendByte( 0xba );
	serialPortSendByte( 0x98 );
	serialPortSendByte( 0x76 );
	
	//发送亮度值
	serialPortSendByte( getBrightness() );

	// 发送湿度值
	serialPortSendByte( getHumidity(1));
	
	// 取得温度值
	temperature = readTemperature(1);
	
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 


	serialPortSendByte( temperature<0 ? 1 : 0 ); //如果是零下则此位置1
	serialPortSendByte( intTemperature/10); //温度的整数部分
	serialPortSendByte( intTemperature%10); //温度的小数部分
	
	serialPortSendByte( 0x88 );
	serialPortSendByte( readAM2301() );
	serialPortSendByte( getAM2301Data(0));
	serialPortSendByte( getAM2301Data(1));
	serialPortSendByte( getAM2301Data(2));
	serialPortSendByte( getAM2301Data(3));
	serialPortSendByte( getAM2301Data(4));
	
	//帧尾
	serialPortSendByte( 0x54 );
	serialPortSendByte( 0x32 );
	serialPortSendByte( 0x10 );	
}

//通过24L01将数据发送给主机
void sendHTDataNRF24L01()
{
	
	byte txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;

	byte humidity;
	byte brightness;
	
	//湿度
	humidity = getHumidity(1);
	
	//温度
	temperature = readTemperature(1);
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	//亮度
	brightness = getBrightness();
	
	

	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	txData[2] = humidity;
	txData[3] = temperature<0 ? 1 : 0; //如果是零下则此位置1
	txData[4] = intTemperature/10; //温度的整数部分
	txData[5] = intTemperature%10; //温度的小数部分
	txData[6] = brightness; //亮度
	txData[7] = 88;
	txData[8] = readAM2301();
	txData[9] = getAM2301Data(0);
	txData[10] = getAM2301Data(1);
	txData[11] = getAM2301Data(2);
	txData[12] = getAM2301Data(3);
	txData[13] = getAM2301Data(4);
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送

}


void main (void)
{
	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时3秒，让大部分元件稳定下来
	delayMS( 3000 );
	
	//初始化串口
	//serialPortInit();
	
	//初始化ADC
	initADC();
	
	//初始化DHT11
	initDHT11();
	
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
			
			//sendHTDataSerialPort();
		}
	}
}



/*-------------
定时器中断子程序
--------------*/
//定时器0中断处理程序
void interruptTimer0(void) interrupt 1
{
	TR0=0; //关闭定时器0
	ET0=0; //关闭定时器0中断
	TH0=0x4c; //给定时器0重赋初值，定时时间50ms，11.0592MHz晶振
	TL0=0x33; //重装低8位（修正后的值） 
	//1f慢，22慢，24慢，30慢，40快，36快，32快，28快, 24快，20快，1a快，00稍慢
	// 2011.11.30 TH0=0x4c TL0=0x00 准了 sendDataInterval=10s 11.0592M STC12C5608AD
	
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
