#ifndef __BMP085_H__
#define __BMP085_H__

// ��������
#define	BMP085_SlaveAddress   0xee	  //����������IIC�����еĴӵ�ַ 							  
#define OSS 3	// Oversampling Setting 

// ���Ŷ���
sbit SCL = P1^2; //IICʱ�����Ŷ���
sbit SDA = P1^1; //IIC�������Ŷ���
sbit EOC = P1^4; //End Of Conversion

// ��������
void bmp085Init();
void bmp085Convert();
long bmp085GetTemperature();
long bmp085GetPressure();

#endif