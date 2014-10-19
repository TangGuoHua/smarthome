/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2011年09月25日  黄长浩  初始版本
2013年10月14日  黄长浩  修改延时函数，去掉对Delay.h的依赖

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>     //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include "ds18B20.h"

/*------------------------------------------------
                  函数声明
------------------------------------------------*/
unsigned char readOneByte(void);
void writeOneByte(unsigned char writeByte);


// 延时函数
void ds18B20Delay50us(void)   //@4MHz Crystal
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=47;a>0;a--);
}

void ds18B20Delay600us(void)   //@4MHz Crystal
{
    unsigned char a,b;
    for(b=171;b>0;b--)
        for(a=2;a>0;a--);
}

void ds18B20Delay120us(void)   //@4MHz Crystal
{
    unsigned char a,b;
    for(b=3;b>0;b--)
        for(a=38;a>0;a--);
}

void ds18B20Delay1s(void)   //@4MHz Crystal
{
    unsigned char a,b,c;
    for(c=127;c>0;c--)
        for(b=235;b>0;b--)
            for(a=32;a>0;a--);
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
	ds18B20Delay50us();//稍做延时
	
	DS18B20 = 0;    //单片机将DQ拉低
	ds18B20Delay600us(); //精确延时 大于 480us 小于960us
	DS18B20 = 1;     //拉高总线
	ds18B20Delay120us();  //15~60us 后 接收60-240us的存在脉冲
	returnData=~DS18B20; //如果x=1则初始化成功, x=0则初始化失败

	ds18B20Delay50us();//稍作延时返回

	
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
		
		ds18B20Delay50us();
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
		ds18B20Delay50us();
		DS18B20 = 1;
		writeByte >>= 1;
	}
	ds18B20Delay50us();
}


// 读取温度

// 有两种模式：0和1
// 模式0：调用readTemperature(0)实际取得的是上次读取时的测得温度值
// 模式1：调用readTemperature(1)将取得当前的温度值，但是需要1s多后函数才返回。
// 返回是个浮点数（温度值） 

float readTemperature(bit mode)
{
	unsigned char tempH, tempL;
	float returnData;
	
	initDS18B20();
	writeOneByte( 0xCC ); // 跳过读序号列号的操作
	writeOneByte( 0x44 ); // 启动温度转换
	
	if( mode )
	{
		ds18B20Delay1s(); //12位分辨率，需要750ms测温
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
