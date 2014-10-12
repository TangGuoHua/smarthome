/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年01月26日  黄长浩  初始版本
2014年10月07日  黄长浩  调整一些延时函数

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/
//
// STC11F04E 1T单片机 4MHz晶振 调试成功
//

#include <reg52.h>
#include "am2321.h"

static unsigned char AM2321_Data[5]={0x00,0x00,0x00,0x00,0x00};

void am2321Delay2S(void)
{
	//晶振为4MHz时 1T STC11F04E
	//延时2.5s
    unsigned char a,b,c,n;
    for(c=165;c>0;c--)
        for(b=100;b>0;b--)
            for(a=150;a>0;a--);
    for(n=1;n>0;n--);
}

void am2321DelayMS()
{
	//晶振为4MHz时 1T STC11F04E
	//延时2ms
    unsigned char a,b,c;
    for(c=7;c>0;c--)
        for(b=8;b>0;b--)
            for(a=34;a>0;a--);
}


/******************
函数功能: 延时40us
*******************/
void am2321DelayUS()
{  
	//晶振为4MHz时 1T STC11F04E
	//延时40us
    unsigned char a,b;
    for(b=11;b>0;b--)
        for(a=2;a>0;a--);
} 

//数据清零
void clearData(void)
{
	int i;
	for(i=0;i<5;i++)
	{
		AM2321_Data[i] = 0x00;
	}
}




// 从AM2321读取一个字节的数据
unsigned char am2321ReadOneByte(void)
{
	unsigned char i, temp, returnData;
	unsigned int loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 9000;
		while( !AM2321_SDA && loopCount-- ); //检测低电平是否结束
		
		//延时Min=26us Max50us, 跳过数据"0"的高电平
		am2321DelayUS(); //延时40us
		temp= AM2321_SDA?1:0;

		loopCount = 9000;
		while( AM2321_SDA && loopCount-- ); //等待高电平结束
		//超时则跳出for循环		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}



//初始化AM2321
//需要2.5秒后才能返回
void initAM2321()
{
	AM2321_SDA = 1;
	am2321Delay2S();  //延时2.5s
}


/********************************************
功能： 读传感器

参数：mode - 读取模式
  1 - 读取1次AM2321的值 。返回快 但返回值是上一次测量的值
  2 - 先触发一次AM2321采样，等待2.9秒，再读取一次AM2321。返回需要3秒，但返回值是本次测量的值

返回值：
  0 - 读取AM2321成功
  1 - AM2321未响应（传感器没有接）
  2 - 读出的数据校验和出错       
  3,4 - 读取出错 （General error）
  5 - 读取出错（所有读数为零）
********************************************/
unsigned char readAM2321(unsigned char mode ) 
{
	unsigned char result = 0;
	unsigned char i;
	unsigned char checkSum;
	unsigned int cnt=0;
	unsigned int tmp;
	
	//清除数据
	clearData();
	
	//主机拉低(Min=800US Max=20Ms) 
	AM2321_SDA = 0;
	am2321DelayMS();  //延时2Ms
	//释放总线 延时(Min=30us Max=50us)
	AM2321_SDA = 1;
	
	if( mode==2)
	{
		am2321Delay2S();
		AM2321_SDA = 0;
		am2321DelayMS();  //延时2Ms
		//释放总线 延时(Min=30us Max=50us)
		AM2321_SDA = 1;
	}

	am2321DelayUS(); //延时40us
	
	
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行 
	if(AM2321_SDA ==0)
	{
		//收到起始信号
		
		//判断从机是否发出 80us 的低电平响应信号是否结束
		cnt=0;
		while(!AM2321_SDA)
		{
			if(++cnt>9000) //防止进入死循环
			{
				//读取出错
				return 3;
			} 
		}
		
		cnt = 0;
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
		while((AM2321_SDA))
		{
			if(++cnt>9000) //防止进入死循环
			{
				//读取出错
				return 4;
			}
		}
		
		// 数据接收	传感器共发送40位数据
		// 即5个字节 高位先送  5个字节分别为湿度高位 湿度低位 温度高位 温度低位 校验和
		// 校验和为：湿度高位+湿度低位+温度高位+温度低位
		for(i=0;i<5;i++)
		{
			AM2321_Data[i] = am2321ReadOneByte();
		}
		
		checkSum = AM2321_Data[0]+AM2321_Data[1]+AM2321_Data[2]+AM2321_Data[3];
		
		if( checkSum == 0 )
		{
			//读取出错，一般来说温湿度不会同时为0
			return 5;
		}
		
		if( AM2321_Data[4] != checkSum )
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
	tmp = AM2321_Data[0]*256 + AM2321_Data[1];
	AM2321_Data[0] = tmp/10; //取得整数部分
	AM2321_Data[1] = tmp%10; //取得小数部分
	
	//温度
	if( (AM2321_Data[2] & 0x80) == 0x80 )
	{
		AM2321_Data[4]=1;
		AM2321_Data[2] = AM2321_Data[2] & 0x7f;
	}
	else
	{
		AM2321_Data[4]=0;
	}
	
	tmp = AM2321_Data[2]*256 + AM2321_Data[3];
	AM2321_Data[2] = tmp/10; //取得整数部分
	AM2321_Data[3] = tmp%10; //取得小数部分
	
	return result;
}


/********************************
功能：取得从AM2321读出的值

首先，调用readAM2321()方法，并确保其返回值为0（读取AM2321成功）。
否则调用本方法得到的数据是不正确的。

Index   Value 
-----------------------------------
0       湿度整数部分
1       湿度小数部分
2       温度整数部分
3       温度小数部分
4       负温度标记 （0:正温度，1:负温度）

例1：湿度53.8% 温度18.3，返回值如下：
getAM2321Data(0)  53
getAM2321Data(1)  8
getAM2321Data(2)  18
getAM2321Data(3)  3
getAM2321Data(4)  0 （0表示正温度）

例2：湿度20.5% 温度-2.6，返回值如下：
getAM2321Data(0)  20
getAM2321Data(1)  5
getAM2321Data(2)  2
getAM2321Data(3)  6
getAM2321Data(4)  1 （1表示负温度）

*********************************/
unsigned char getAM2321Data( unsigned char index )
{
	return AM2321_Data[index];
}
