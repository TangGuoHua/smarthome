#ifndef __BMP085_H__
#define __BMP085_H__

// 常量定义
#define	BMP085_SlaveAddress   0xee	  //定义器件在IIC总线中的从地址 							  
#define OSS 3	// Oversampling Setting 

// 引脚定义
sbit SCL = P1^2; //IIC时钟引脚定义
sbit SDA = P1^1; //IIC数据引脚定义
sbit EOC = P1^4; //End Of Conversion

// 函数声明
void bmp085Init();
void bmp085Convert();
long bmp085GetTemperature();
long bmp085GetPressure();

#endif