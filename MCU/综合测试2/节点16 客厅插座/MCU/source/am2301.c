//
// 晶振为11.0592M时  STC12C5608AD 1T单片机调试成功
//

#include <reg52.h>
#include <intrins.h>
#include "am2301.h"

static unsigned char AM2301_Data[5]={0x00,0x00,0x00,0x00,0x00};


/********************************************\
|* 功能： 延时t毫秒
|* 延时大约 1ms
\********************************************/ 
void am2301DelayMS(unsigned int t)
{
	unsigned int i;
	unsigned int j;
	for(j=t;j>0;j--)
		//for(i=848;i>0;i--);  //1T STC12C5608AD 晶振为11.0592M
		//for( i=530; i>0; i-- ); // 1T STC11F04E 内部RC 6.97MHz
		for( i=306; i>0; i-- ); // 1T STC12C5608AD 外部晶振 4MHz
}



/****************************************************
函数功能: 延时30us

*****************************************************/
void am2301DelayUS()
{  

    unsigned char a,b;
	//1T STC12C5608AD 外部晶振 4MHz 以下代码适用： 
    for(b=3;b>0;b--)
    	for(a=8;a>0;a--);
	
//1T STC11F04E 内部RC 6.97MHz 以下代码适用： 
//    for(b=1;b>0;b--)
//        for(a=49;a>0;a--);
//    _nop_();  //if Keil,require use intrins.h

} 

//数据清零
void am2301ClearData(void)
{
	int i;
	for(i=0;i<5;i++)
	{
		AM2301_Data[i] = 0x00;
	}
}

void initAM2301()
{
	AM2301_SDA = 1;
}

// 从AM2301读取一个字节的数据
unsigned char am2301Read_SensorData(void)
{
	unsigned char i, temp, returnData, loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 500;
		while( !AM2301_SDA && loopCount-- ); //检测低电平是否结束
		
		//延时Min=26us Max50us, 跳过数据"0"的高电平
		am2301DelayUS(); //延时30us
		temp= AM2301_SDA?1:0;

		loopCount = 500;
		while( AM2301_SDA && loopCount-- ); //等待高电平结束
		//超时则跳出for循环		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}



/********************************************
功能： 读传感器

参数：mode - 读取模式
  1 - 读取1次AM2301的值 。返回快 但返回值是上一次测量的值
  2 - 先触发一次AM2301采样，等待2.9秒，再读取一次AM2301。返回需要3秒，但返回值是本次测量的值

返回值：
  0 - 读取AM2301成功
  1 - AM2301未响应（传感器没有接）
  2 - 读出的数据校验和出错       
  3 - 读取出错 （General error）

********************************************/
unsigned char readAM2301( unsigned char mode )
{
	unsigned char result = 0;
	unsigned char i;
	unsigned char checkSum;
	unsigned int cnt=0;
	unsigned int tmp;
	
	//清除数据
	am2301ClearData();
	
	//主机拉低(Min=800US Max=20Ms) 
	AM2301_SDA = 0;
	am2301DelayMS(3);  //延时2Ms
	//释放总线 延时(Min=30us Max=50us)
	AM2301_SDA = 1;
	
	if( mode==2)
	{
		am2301DelayMS(2900);
		AM2301_SDA = 0;
		am2301DelayMS(3);  //延时2Ms
		//释放总线 延时(Min=30us Max=50us)
		AM2301_SDA = 1;
	}

	am2301DelayUS(); //延时30us
	
	
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行 
	if(AM2301_SDA ==0)
	{
	
		//收到起始信号
		
		//判断从机是否发出 80us 的低电平响应信号是否结束
		cnt=0;
		while((!AM2301_SDA))
		{
			if(++cnt>500) //防止进入死循环
			{
				//读取出错
				return 3;
			} 
		}
		
		cnt = 0;
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
		while((AM2301_SDA))
		{
			if(++cnt>500) //防止进入死循环
			{
				//读取出错
				return 3;
			}
		}
		// 数据接收	传感器共发送40位数据
		// 即5个字节 高位先送  5个字节分别为湿度高位 湿度低位 温度高位 温度低位 校验和
		// 校验和为：湿度高位+湿度低位+温度高位+温度低位
		for(i=0;i<5;i++)
		{
			AM2301_Data[i] = am2301Read_SensorData();
		}
		
		checkSum = AM2301_Data[0]+AM2301_Data[1]+AM2301_Data[2]+AM2301_Data[3];
		
		if( checkSum == 0 )
		{
			//读取出错，一般来说温湿度不会同时为0
			return 3;
		}
		
		if( AM2301_Data[4] != checkSum )
		{
			//校验和出错
			return 2;
		}
	}
	else
	{
		return 1; // 未收到传感器响应
	}
	
	//运行到这里，数据已经被正确地读出了
	//下面处理数据
	
	//湿度
	tmp = AM2301_Data[0]*256 + AM2301_Data[1];
	AM2301_Data[0] = tmp/10; //取得整数部分
	AM2301_Data[1] = tmp%10; //取得小数部分
	
	//温度
	if( (AM2301_Data[2] & 0x80) == 0x80 )
	{
		AM2301_Data[4]=1;
		AM2301_Data[2] = AM2301_Data[2] & 0x7f;
	}
	else
	{
		AM2301_Data[4]=0;
	}
	
	tmp = AM2301_Data[2]*256 + AM2301_Data[3];
	AM2301_Data[2] = tmp/10; //取得整数部分
	AM2301_Data[3] = tmp%10; //取得小数部分
	
	return result;
}


/********************************
功能：取得从AM2301读出的值

首先，调用readAM2301()方法，并确保其返回值为0（读取AM2301成功）。
否则调用本方法得到的数据是不正确的。

Index   Value 
-----------------------------------
0       湿度整数部分
1       湿度小数部分
2       温度整数部分
3       温度小数部分
4       负温度标记 （0:正温度，1:负温度）

例1：湿度53.8% 温度18.3，返回值如下：
getAM2301Data(0)  53
getAM2301Data(1)  8
getAM2301Data(2)  18
getAM2301Data(3)  3
getAM2301Data(4)  0 （0表示正温度）

例2：湿度20.5% 温度-2.6，返回值如下：
getAM2301Data(0)  20
getAM2301Data(1)  5
getAM2301Data(2)  2
getAM2301Data(3)  6
getAM2301Data(4)  1 （1表示负温度）

*********************************/
unsigned char getAM2301Data( unsigned char index )
{
	return AM2301_Data[index];
}
