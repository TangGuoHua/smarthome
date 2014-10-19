/***********************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振

【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2012年09月18日  黄长浩  初始版本
2014年10月19日  黄长浩  调整延时程序，适应4MHz晶振

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
************************************************************************/

#include <REG52.H>	
#include "bh1750fvi.h"

unsigned char bh1750Buffer[4]; //接收数据缓存区

/**************************************
延时5微秒
使用单片机11F04E @4MHz
**************************************/
void BH1750Delay5us()
{
	unsigned char a,b; //4MHz晶振
    for(b=1;b>0;b--)
        for(a=2;a>0;a--);
}


/**************************************
延时180毫秒
使用单片机11F04E @4MHz
**************************************/
void BH1750Delay180ms(void)
{
    unsigned char a,b,c;
    for(c=33;c>0;c--)
        for(b=82;b>0;b--)
            for(a=65;a>0;a--);		
}


/**************************************
起始信号
**************************************/
void BH1750_Start()
{
    BH1750_SDA = 1;                    //拉高数据线
    BH1750_SCL = 1;                    //拉高时钟线
    BH1750Delay5us();                 //延时
    BH1750_SDA = 0;                    //产生下降沿
    BH1750Delay5us();                 //延时
    BH1750_SCL = 0;                    //拉低时钟线
}


/**************************************
停止信号
**************************************/
void BH1750_Stop()
{
    BH1750_SDA = 0;                    //拉低数据线
    BH1750_SCL = 1;                    //拉高时钟线
    BH1750Delay5us();                 //延时
    BH1750_SDA = 1;                    //产生上升沿
    BH1750Delay5us();                 //延时
}


/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(bit ack)
{
    BH1750_SDA = ack;                  //写应答信号
    BH1750_SCL = 1;                    //拉高时钟线
    BH1750Delay5us();                 //延时
    BH1750_SCL = 0;                    //拉低时钟线
    BH1750Delay5us();                 //延时
}


/**************************************
接收应答信号
**************************************/
bit BH1750_RecvACK()
{
    BH1750_SCL = 1;                    //拉高时钟线
    BH1750Delay5us();                 //延时
    CY = BH1750_SDA;                   //读应答信号
    BH1750_SCL = 0;                    //拉低时钟线
    BH1750Delay5us();                 //延时

    return CY;
}


/**************************************
向IIC总线发送一个字节数据
**************************************/
void BH1750_SendByte(unsigned char dat)
{
    unsigned char i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        BH1750_SDA = CY;               //送数据口
        BH1750_SCL = 1;                //拉高时钟线
        BH1750Delay5us();             //延时
        BH1750_SCL = 0;                //拉低时钟线
        BH1750Delay5us();             //延时
    }
    BH1750_RecvACK();
}


/**************************************
从IIC总线接收一个字节数据
**************************************/
unsigned char BH1750_RecvByte()
{
    unsigned char i;
    unsigned char dat = 0;

    BH1750_SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        BH1750_SCL = 1;                //拉高时钟线
        BH1750Delay5us();             //延时
        dat |= BH1750_SDA;             //读数据               
        BH1750_SCL = 0;                //拉低时钟线
        BH1750Delay5us();             //延时
    }
    return dat;
}


//单个写入数据
void BH1750_SingleWrite(unsigned char REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
	//BH1750_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    BH1750_Stop();                   //发送停止信号
}


//单个字节读取内部寄存器数据
/*
uchar Single_Read_BH1750(uchar REG_Address)
{  uchar REG_data;
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    BH1750_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=BH1750_RecvByte();              //读出寄存器数据
	BH1750_SendACK(1);   
	BH1750_Stop();                           //停止信号
    return REG_data; 
}
*/


//连续读取内部寄存器数据
void BH1750_MultipleRead(void)
{
	unsigned char i;
	
	BH1750_Start();                          //起始信号
	BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	for (i=0; i<3; i++)                      //连续读取3个地址数据，存储中BUF
	{
		bh1750Buffer[i] = BH1750_RecvByte();  //BUF[0]存储0x32地址中的数据
		if (i == 3)
		{
			BH1750_SendACK(1);                //最后一个数据需要回NOACK
		}
		else
		{		
			BH1750_SendACK(0);                //回应ACK
		}
	}
	BH1750_Stop();                          //停止信号
	//Delay5ms();
}


//初始化BH1750
void bh1750Init()
{
   BH1750_SingleWrite(0x01);  //Power ON
   BH1750_SingleWrite(0x10);  //高分辨率连续测量模式
   BH1750Delay180ms(); //延时180ms  因为一次高分辨率的测量最长耗时180ms，初始化后延时180ms就可以让以后读到的值都是有效的值
}


//返回亮度值
unsigned int bh1750GetBrightness()
{  
	unsigned int tmp;

    BH1750_MultipleRead(); //连续读出数据，存储在BUF中

    tmp = bh1750Buffer[0];
    tmp = (tmp<<8) + bh1750Buffer[1]; //合成数据 
    tmp = (float)tmp/1.2;
	return tmp;
} 

