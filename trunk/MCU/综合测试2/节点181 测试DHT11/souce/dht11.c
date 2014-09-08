/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
奥松DHT11温湿度传感器


【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2014年09月04日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include "dht11.h"

static unsigned char DHT11_HUMIDITY=255;
static unsigned char DHT11_TEMPERATURE=255;

void dht11Delay10Us()
{
    unsigned char a;
    for(a=7;a>0;a--);
}


void dht11Delay20Ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=203;b>0;b--)
            for(a=97;a>0;a--);
}

void dht11Delay5S(void)   //误差 0us
{
    unsigned char a,b,c,n;
    for(c=189;c>0;c--)
        for(b=191;b>0;b--)
            for(a=137;a>0;a--);
    for(n=3;n>0;n--);

}

// 从DTH11读取一个字节的数据
unsigned char getOneByte(void)
{
	unsigned char i, temp, returnData;
	unsigned int loopCount;
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 2000;
		while( !DHT11 && loopCount-- );
		
		//delay 30us
		dht11Delay10Us();
		dht11Delay10Us();
		dht11Delay10Us();
		
		temp= DHT11?1:0;

		loopCount = 2000;
		while( DHT11 && loopCount-- );
		//超时则跳出for循环		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}

//触发一次测量
void triggerMesurment()
{
	//主机拉低20ms (要求大于18ms)
	DHT11=0;
	dht11Delay20Ms();
	DHT11=1;
}


/////////以下部分函数供外部程序调用/////////////////////

/********************************************
功能： 读传感器

参数：mode - 读取模式
  1 - 读取DHT11的值，并立即返回 。
      注意，通过本模式读取DHT11后，调用getDHT11Humidity()和getDHT11Temperature()得到返回值是上一次测量的值
	  
  2 - 先触发一次DHT11采样，等待5秒，再读取DHT11。
      注意，通过本模式读取DHT11需要5秒钟才能返回。
	  但调用getDHT11Humidity()和getDHT11Temperature()得到本次测量的值

返回值：
  0 - 读取DHT11成功
  1 - DHT11未响应（传感器没有接）
  2 - 读出的数据校验和出错       
  3 - 读取出错 （General error）
  
注意，一定要保证readDHT11()返回值为0时，才能通过调用getDHT11Humidity()和getDHT11Temperature()得到正确的温湿度值。
否则getDHT11Humidity()和getDHT11Temperature()将返回255，表示读取DHT11时出错了
**********************************************************************************************************/
unsigned char readDHT11( unsigned char mode)
{
	unsigned int loopCount;
	unsigned char temp;
	unsigned char tempHHigh, tempHLow, tempTHigh, tempTLow, tempCheckSum;
	
	//如果读取DHT11失败，则getDHT11Humidity()和getDHT11Temperature()将返回255
	DHT11_HUMIDITY = 255;
	DHT11_TEMPERATURE = 255;

	if(mode==2)
	{
		triggerMesurment();
		dht11Delay5S();
	}
	
	//主机拉低20ms (要求大于18ms)
	DHT11=0;
	dht11Delay20Ms();
	DHT11=1;
	
	//总线由上拉电阻拉高 主机延时20us
	dht11Delay10Us();
	dht11Delay10Us();
	
	//主机设为输入 判断从机响应信号 
	DHT11=1;
	dht11Delay10Us();
	
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	if(!DHT11)
	{
		loopCount = 2000;
		//判断从机是否发出 80us 的低电平响应信号是否结束，或是否超时	 
		while( !DHT11 && --loopCount );
		if( loopCount==0 )
		{
			//超时
			return 3;
		}
		
		loopCount = 2000;
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态，或是否超时
		while( DHT11 && --loopCount  );
		if( loopCount==0 )
		{
			//超时
			return 3;
		}
				
		//数据接收状态，收取5个字节
		tempHHigh    = getOneByte();
		tempHLow     = getOneByte();
		tempTHigh    = getOneByte();
		tempTLow     = getOneByte();
		tempCheckSum = getOneByte();
		DHT11=1;
		
		//数据校验 
		temp = tempHHigh + tempHLow + tempTHigh + tempTLow;
		if( temp == tempCheckSum && temp>0 )
		{
			// 读取数据有效，成功
			DHT11_HUMIDITY = tempHHigh;
			DHT11_TEMPERATURE = tempTHigh;
			
			return 0;
		}
	}

	//从机没有响应
	return 1;
}


//取得湿度值
//注意，一定要readDHT11()返回值为0时，才能通过调用getDHT11Humidity()得到正确的湿度值
//否则getHumidity()将返回255
unsigned char getDHT11Humidity()
{
	return DHT11_HUMIDITY;
}


//取得温度值
//注意，一定要readDHT11()返回值为0时，才能通过调用getDHT11Temperature()得到正确的温度值
//否则getDHT11Temperature()将返回255
unsigned char getDHT11Temperature()
{
	return DHT11_TEMPERATURE;
}


//初始化DHT11
void initDHT11()
{
	DHT11 = 1;
}
