/*
When        Who        Remarks
----------------------------------------------------------------------
2011-SEP-25 Changhao   Initial version
2011-OCT-07 Changhao   延长getHumidity()方法中两次采样间隔时间到2800ms。（刚才检测到一次数据出错，我怀疑是采样间隔时间不够造成的）
*/

#include <reg52.h>
#include "dht11.h"

/********************************************\
|* 功能： 延时	
|* 延时大约 1ms
\********************************************/ 
void dth11DelayMS(unsigned int t)
{
	unsigned int i;
	unsigned int j;
	for(j=t;j>0;j--)
		//for(i=848;i>0;i--);  //1T STC12C5608AD 晶振为11.0592M
		//for( i=530; i>0; i-- ); // 1T STC11F04E 内部RC 6.97MHz
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M晶振
}



/****************************************************
函数功能: 延时30us
// 1T STC12C5608AD 4M晶振
*****************************************************/
void dht11Delay30us(void)   //误差 0us
{
    unsigned char a;
    for(a=27;a>0;a--);
}

// 从DTH11读取一个字节的数据
unsigned char getOneByte(void)
{
	unsigned char i, temp, returnData, loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 2000;
		while( !DHT11 && loopCount-- );
		dht11Delay30us();
		temp= DHT11?1:0;

		loopCount = 2000;
		while( DHT11 && loopCount-- );
		//超时则跳出for循环		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}

// 读取DHT11传送的5个字节，但只返回湿度值
unsigned char getAllBytes(void)
{
	unsigned char temp, loopCount;
	unsigned char tempHHigh, tempHLow, tempTHigh, tempTLow, tempCheckSum;
	unsigned char returnData;
	
	//如果读取湿度不成功，则返回0xff，否则返回相对湿度值
	returnData = 0xff;
	
	//主机拉低20ms (要求大于18ms)
	DHT11=0;
	dth11DelayMS(20);
	DHT11=1;
	//总线由上拉电阻拉高 主机延时20us
	dht11Delay30us();
	//主机设为输入 判断从机响应信号 
	DHT11=1;
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	if(!DHT11)
	{
		loopCount = 2000;
		//判断从机是否发出 80us 的低电平响应信号是否结束，或是否超时	 
		while( !DHT11 && loopCount-- );
		
		loopCount = 2000;
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态，或是否超时
		while( DHT11 && loopCount--  );
		//数据接收状态		 

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
			// 数据有效 返回湿度
			returnData = tempHHigh;
		}

	}// end if
	
	return returnData;

}

//--------------------------------
//-----湿度读取子程序 ------------
//--------------------------------
// 取得湿度值
// 有两种模式：0和1
// 模式0：调用getHumidity(0)实际取得的是上次读取时的测得湿度值，函数立即返回。
// 模式1：调用getHumidity(1)将取得当前的湿度值，但是函数需要2秒以上才返回。
// 如果读取湿度不成功，则返回0xff，否则返回相对湿度值
unsigned char getHumidity(bit mode)
{
	if( mode )
	{
	 	//每次读取DHT11的操作将返回上次测量的值，并触发一次测量
	 	//所以，为了得到最新的温湿度值，下面是连续2次读取，以获得最新值
		getAllBytes();
		dth11DelayMS(2800); // 两次读取间隔建议在2秒以上
	}
	return getAllBytes();
}

//初始化DHT11
void initDHT11(void)
{
	DHT11 = 1;
}
