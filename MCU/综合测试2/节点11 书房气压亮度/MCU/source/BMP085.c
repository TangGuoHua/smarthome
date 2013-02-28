/********************
BMP085气压传感模块
适用环境: STC11F04E + 3.6864M Crystal
by HUANG CHANGHAO
on 2013-JAN-3
********************/
#include <reg52.h>
#include "BMP085.h"

//bmp085校正参数
idata short ac1;
idata short ac2; 
idata short ac3; 
idata unsigned short ac4;
idata unsigned short ac5;
idata unsigned short ac6;
idata short b1; 
idata short b2;
idata short mb;
idata short mc;
idata short md;

// 变量
static long bmp085Temperature;
static long bmp085Pressure;


/**************************************
延时5微秒
不同的工作环境,需要调整此函数
注意时钟过快时需要修改
单片机:STC11F04E
**************************************/
void BMP085Delay5us()
{
	unsigned char a;
	//for(a=3;a>0;a--); // 晶振：3.6864MHz
	for(a=6;a>0;a--); //内部RC振荡: 6.22MHz
}


/**************************************
起始信号
**************************************/
void BMP085_Start()
{
	SDA = 1;					//拉高数据线
	SCL = 1;					//拉高时钟线
	BMP085Delay5us(); 				//延时
	SDA = 0;					//产生下降沿
	BMP085Delay5us(); 				//延时
	SCL = 0;					//拉低时钟线
}

/**************************************
停止信号
**************************************/
void BMP085_Stop()
{
	SDA = 0;					//拉低数据线
	SCL = 1;					//拉高时钟线
	BMP085Delay5us(); 				//延时
	SDA = 1;					//产生上升沿
	BMP085Delay5us(); 				//延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(bit ack)
{
	SDA = ack;					//写应答信号
	SCL = 1;					//拉高时钟线
	BMP085Delay5us(); 				//延时
	SCL = 0;					//拉低时钟线
	BMP085Delay5us(); 				//延时
}

/**************************************
接收应答信号
**************************************/
bit BMP085_RecvACK()
{
	SCL = 1;					//拉高时钟线
	BMP085Delay5us(); 				//延时
	CY = SDA;					//读应答信号
	SCL = 0;					//拉低时钟线
	BMP085Delay5us(); 				//延时

	return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BMP085_SendByte(unsigned char dat)
{
	unsigned char i;

	for (i=0; i<8; i++) 		//8位计数器
	{
		dat <<= 1;				//移出数据的最高位
		SDA = CY;				//送数据口
		SCL = 1;				//拉高时钟线
		BMP085Delay5us(); 			//延时
		SCL = 0;				//拉低时钟线
		BMP085Delay5us(); 			//延时
	}
	BMP085_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
unsigned char BMP085_RecvByte()
{
	unsigned char i;
	unsigned char dat = 0;

	SDA = 1;					//使能内部上拉,准备读取数据,
	for (i=0; i<8; i++) 		//8位计数器
	{
		dat <<= 1;
		SCL = 1;				//拉高时钟线
		BMP085Delay5us(); 			//延时
		dat |= SDA; 			//读数据			   
		SCL = 0;				//拉低时钟线
		BMP085Delay5us(); 			//延时
	}
	return dat;
}

//********************************
//读出BMP085内部数据,连续两个字节
//*********************************
short bmp085Read2Bytes(unsigned char ST_Address)
{	
	unsigned char msb, lsb;
	short _data;
	BMP085_Start(); 						 //起始信号
	BMP085_SendByte(BMP085_SlaveAddress);	 //发送设备地址+写信号
	BMP085_SendByte(ST_Address);			 //发送存储单元地址
	BMP085_Start(); 						 //起始信号
	BMP085_SendByte(BMP085_SlaveAddress+1); 		//发送设备地址+读信号

	msb = BMP085_RecvByte();				 //BUF[0]存储
	BMP085_SendACK(0);						 //回应ACK
	lsb = BMP085_RecvByte();	 
	BMP085_SendACK(1);						 //最后一个数据需要回NOACK

	BMP085_Stop();							 //停止信号
	//Delay5ms();
	_data = msb << 8;
	_data |= lsb;	
	return _data;
}

//*******************************
long bmp085ReadUT(void)
{

	BMP085_Start(); 				 //起始信号
	BMP085_SendByte(BMP085_SlaveAddress);	//发送设备地址+写信号
	BMP085_SendByte(0xF4);			  // write register address
	BMP085_SendByte(0x2E);		 	// write register data for temp
	BMP085_Stop();					 //发送停止信号

	//Delay5ms();  //至少需要等待4.5ms
	while(!EOC); // 等待转换完成。转换完成，EOC=1；转换进行中，EOC=0
		
	return (long) bmp085Read2Bytes(0xF6);
}

//*****************************
long bmp085ReadUP(void)
{
	//long pressure = 0;
	unsigned char msb, lsb, xlsb;
	unsigned long up = 0;	
	
	// Write 0x34+(OSS<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	BMP085_Start(); 				  //起始信号
	BMP085_SendByte(BMP085_SlaveAddress);	//发送设备地址+写信号
	BMP085_SendByte(0xF4);			  // write register address
	BMP085_SendByte(0x34 + (OSS<<6)); // write register data for pressure
	BMP085_Stop();					  //发送停止信号

	// Wait for conversion, delay time dependent on OSS
	//delay(2 + (3<<OSS));
	//Delay5ms();
	while(!EOC); // 等待转换完成。转换完成，EOC=1；转换进行中，EOC=0
	
	// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
	BMP085_Start(); 						 //起始信号
	BMP085_SendByte(BMP085_SlaveAddress);	 //发送设备地址+写信号
	BMP085_SendByte(0xf6);			 //发送存储单元地址
	BMP085_Start(); 						 //起始信号
	BMP085_SendByte(BMP085_SlaveAddress+1); 		//发送设备地址+读信号

	msb = BMP085_RecvByte();
	BMP085_SendACK(0);		//回应ACK
	lsb = BMP085_RecvByte();	 
	BMP085_SendACK(0);						 
	xlsb = BMP085_RecvByte();	 
	BMP085_SendACK(1);	 //最后一个数据需要回NOACK

	BMP085_Stop();							 //停止信号
	
	up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
	
	return up;
}

//**************************************************************

//初始化BMP085
void bmp085Init()
{
	ac1 = bmp085Read2Bytes(0xAA);
	ac2 = bmp085Read2Bytes(0xAC);
	ac3 = bmp085Read2Bytes(0xAE);
	ac4 = bmp085Read2Bytes(0xB0);
	ac5 = bmp085Read2Bytes(0xB2);
	ac6 = bmp085Read2Bytes(0xB4);
	b1 =  bmp085Read2Bytes(0xB6);
	b2 =  bmp085Read2Bytes(0xB8);
	mb =  bmp085Read2Bytes(0xBA);
	mc =  bmp085Read2Bytes(0xBC);
	md =  bmp085Read2Bytes(0xBE);
}

// 开始测气温和气压
// 测量是先测气温，在测气压的
// 测量气温需要4.5ms
// 测量气压需要4.5-25.5ms不等（依赖于头文件厘米OSS的设置：OSS=0 4.5ms, OSS=1 7.5ms, OSS=2 13.5ms, OSS=3 25.5ms)
void bmp085Convert()
{
	long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	
	ut = bmp085ReadUT(); // 读取温度
	up = bmp085ReadUP(); // 读取压强
	
	//计算温度
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	bmp085Temperature = (b5 + 8) >> 4;  // temperature * 0.1 摄氏度

	//计算气压
	b6 = b5 - 4000;  //注意：b5是由前面计算温度时算出来的一个参数。因此算气压前必须测温度.
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2; //b3 = (((long)ac1 * 4 + x3) + 2)/4;
	
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	
	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
		p = (b7 * 2) / b4 ;
	else  
		p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	bmp085Pressure = p + ((x1 + x2 + 3791) >> 4); // 单位(Pa)  [1Pa=0.01hPa=0.01mbar]
}

// 得到最近一次测量的气温
// (返回值*0.1)即可得到摄氏度
// 例如：当前气温为25.3摄氏度，那么返回值为253
long bmp085GetTemperature()
{
	return bmp085Temperature;
}

// 得到最近一次测量的气压
// 单位为Pa  [1Pa=0.01hPa=0.01mbar]
long bmp085GetPressure()
{
	return bmp085Pressure;
}