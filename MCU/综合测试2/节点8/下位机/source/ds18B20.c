/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include <reg52.h>     //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include "ds18B20.h"

/*------------------------------------------------
                  函数声明
------------------------------------------------*/
unsigned char readOneByte(void);
void writeOneByte(unsigned char writeByte);

//延时10us 4M晶振 STC12C5608AD 1T
void ds18B20Delay10us(void)   //误差 0us
{
    unsigned char a;
    for(a=7;a>0;a--);
}

//延时100us 4M晶振 STC12C5608AD 1T
void ds18B20Delay100us(unsigned char b)   //误差 0us
{
    unsigned char a;
    for(;b>0;b--)
        for(a=98;a>0;a--);
}

// 延时1MS 
void ds18B20DelayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M晶振
}


/*--------------
18b20初始化
返回1则初始化成功
返回0则初始化失败
----------------*/
bit initDS18B20(void)
{
	bit returnData;
	
	DS18B20 = 1;    //DQ复位
	ds18B20Delay10us();   //稍做延时
	
	DS18B20 = 0;    //单片机将DQ拉低
	ds18B20Delay100us(6); //精确延时 大于 480us 小于960us
	DS18B20 = 1;     //拉高总线
	ds18B20Delay100us(1);  //15~60us 后 接收60-240us的存在脉冲
	returnData=~DS18B20; //如果x=1则初始化成功, x=0则初始化失败
	
	ds18B20Delay10us();  //稍作延时返回
	ds18B20Delay10us();
	ds18B20Delay10us();
	
	return returnData;
}

/*------------
读取一个字节
-------------*/
unsigned char readOneByte(void)
{
	unsigned char i=0;
	unsigned char returnData = 0;
	for (i=8;i>0;i--)
	{
		DS18B20 = 0; // 给脉冲信号
		returnData>>=1;
		DS18B20 = 1; // 给脉冲信号
		if(DS18B20) returnData|=0x80;
		
		//DelayUs2x(25);
		ds18B20Delay10us();
		ds18B20Delay10us();
		ds18B20Delay10us();
		ds18B20Delay10us();
		ds18B20Delay10us();
	}
	return(returnData);
}
/*-----------
写入一个字节
-----------*/
void writeOneByte( unsigned char writeByte)
{
	unsigned char i=0;
	for (i=8; i>0; i--)
	{
		DS18B20 = 0;
		DS18B20 = writeByte & 0x01;
		//DelayUs2x(25);
		ds18B20Delay10us();
		ds18B20Delay10us();
		ds18B20Delay10us();
		ds18B20Delay10us();
		ds18B20Delay10us();
		DS18B20 = 1;
		writeByte >>= 1;
	}
	//DelayUs2x(25);
	ds18B20Delay10us();
	ds18B20Delay10us();
	ds18B20Delay10us();
	ds18B20Delay10us();
	ds18B20Delay10us();
}


// 读取温度
// 有两种模式：0和1
// 模式0：调用readTemperature(0)实际取得的是上次读取时的测得温度值
// 模式1：调用readTemperature(1)将取得当前的温度值，但是需要约800+ms后函数才返回。
// 返回是个浮点数（温度值） 
float getTemperature(bit mode)
{
	unsigned char tempH, tempL;
	float returnData;
	
	initDS18B20();
	writeOneByte( 0xCC ); // 跳过读序号列号的操作
	writeOneByte( 0x44 ); // 启动温度转换
		
	if( mode )
	{
		ds18B20DelayMS(800); //12位分辨率，需要750ms测温
	}

	initDS18B20();
	writeOneByte( 0xCC ); //跳过读序号列号的操作 
	writeOneByte( 0xBE ); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	
	tempL = readOneByte();   //低位
	tempH = readOneByte();   //高位

	
	if( tempH & 0x80 )
		returnData=(~( tempH * 256 + tempL) + 1) * (-0.0625);  //零度以下, 温度转换，把高位低位做相应的运算转换为实际的温度 
	else
		returnData=( tempH * 256 + tempL ) * 0.0625;   //零度以上
	
	return( returnData );
}
