/********************
BMP085��ѹ����ģ��
���û���: STC11F04E + 3.6864M Crystal
by HUANG CHANGHAO
on 2013-JAN-3
********************/
#include <reg52.h>
#include "BMP085.h"

//bmp085У������
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

// ����
static long bmp085Temperature;
static long bmp085Pressure;


/**************************************
��ʱ5΢��
��ͬ�Ĺ�������,��Ҫ�����˺���
ע��ʱ�ӹ���ʱ��Ҫ�޸�
��Ƭ��:STC11F04E
**************************************/
void BMP085Delay5us()
{
	unsigned char a;
	//for(a=3;a>0;a--); // ����3.6864MHz
	for(a=6;a>0;a--); //�ڲ�RC��: 6.22MHz
}


/**************************************
��ʼ�ź�
**************************************/
void BMP085_Start()
{
	SDA = 1;					//����������
	SCL = 1;					//����ʱ����
	BMP085Delay5us(); 				//��ʱ
	SDA = 0;					//�����½���
	BMP085Delay5us(); 				//��ʱ
	SCL = 0;					//����ʱ����
}

/**************************************
ֹͣ�ź�
**************************************/
void BMP085_Stop()
{
	SDA = 0;					//����������
	SCL = 1;					//����ʱ����
	BMP085Delay5us(); 				//��ʱ
	SDA = 1;					//����������
	BMP085Delay5us(); 				//��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(bit ack)
{
	SDA = ack;					//дӦ���ź�
	SCL = 1;					//����ʱ����
	BMP085Delay5us(); 				//��ʱ
	SCL = 0;					//����ʱ����
	BMP085Delay5us(); 				//��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
bit BMP085_RecvACK()
{
	SCL = 1;					//����ʱ����
	BMP085Delay5us(); 				//��ʱ
	CY = SDA;					//��Ӧ���ź�
	SCL = 0;					//����ʱ����
	BMP085Delay5us(); 				//��ʱ

	return CY;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void BMP085_SendByte(unsigned char dat)
{
	unsigned char i;

	for (i=0; i<8; i++) 		//8λ������
	{
		dat <<= 1;				//�Ƴ����ݵ����λ
		SDA = CY;				//�����ݿ�
		SCL = 1;				//����ʱ����
		BMP085Delay5us(); 			//��ʱ
		SCL = 0;				//����ʱ����
		BMP085Delay5us(); 			//��ʱ
	}
	BMP085_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
unsigned char BMP085_RecvByte()
{
	unsigned char i;
	unsigned char dat = 0;

	SDA = 1;					//ʹ���ڲ�����,׼����ȡ����,
	for (i=0; i<8; i++) 		//8λ������
	{
		dat <<= 1;
		SCL = 1;				//����ʱ����
		BMP085Delay5us(); 			//��ʱ
		dat |= SDA; 			//������			   
		SCL = 0;				//����ʱ����
		BMP085Delay5us(); 			//��ʱ
	}
	return dat;
}

//********************************
//����BMP085�ڲ�����,���������ֽ�
//*********************************
short bmp085Read2Bytes(unsigned char ST_Address)
{	
	unsigned char msb, lsb;
	short _data;
	BMP085_Start(); 						 //��ʼ�ź�
	BMP085_SendByte(BMP085_SlaveAddress);	 //�����豸��ַ+д�ź�
	BMP085_SendByte(ST_Address);			 //���ʹ洢��Ԫ��ַ
	BMP085_Start(); 						 //��ʼ�ź�
	BMP085_SendByte(BMP085_SlaveAddress+1); 		//�����豸��ַ+���ź�

	msb = BMP085_RecvByte();				 //BUF[0]�洢
	BMP085_SendACK(0);						 //��ӦACK
	lsb = BMP085_RecvByte();	 
	BMP085_SendACK(1);						 //���һ��������Ҫ��NOACK

	BMP085_Stop();							 //ֹͣ�ź�
	//Delay5ms();
	_data = msb << 8;
	_data |= lsb;	
	return _data;
}

//*******************************
long bmp085ReadUT(void)
{

	BMP085_Start(); 				 //��ʼ�ź�
	BMP085_SendByte(BMP085_SlaveAddress);	//�����豸��ַ+д�ź�
	BMP085_SendByte(0xF4);			  // write register address
	BMP085_SendByte(0x2E);		 	// write register data for temp
	BMP085_Stop();					 //����ֹͣ�ź�

	//Delay5ms();  //������Ҫ�ȴ�4.5ms
	while(!EOC); // �ȴ�ת����ɡ�ת����ɣ�EOC=1��ת�������У�EOC=0
		
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
	BMP085_Start(); 				  //��ʼ�ź�
	BMP085_SendByte(BMP085_SlaveAddress);	//�����豸��ַ+д�ź�
	BMP085_SendByte(0xF4);			  // write register address
	BMP085_SendByte(0x34 + (OSS<<6)); // write register data for pressure
	BMP085_Stop();					  //����ֹͣ�ź�

	// Wait for conversion, delay time dependent on OSS
	//delay(2 + (3<<OSS));
	//Delay5ms();
	while(!EOC); // �ȴ�ת����ɡ�ת����ɣ�EOC=1��ת�������У�EOC=0
	
	// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
	BMP085_Start(); 						 //��ʼ�ź�
	BMP085_SendByte(BMP085_SlaveAddress);	 //�����豸��ַ+д�ź�
	BMP085_SendByte(0xf6);			 //���ʹ洢��Ԫ��ַ
	BMP085_Start(); 						 //��ʼ�ź�
	BMP085_SendByte(BMP085_SlaveAddress+1); 		//�����豸��ַ+���ź�

	msb = BMP085_RecvByte();
	BMP085_SendACK(0);		//��ӦACK
	lsb = BMP085_RecvByte();	 
	BMP085_SendACK(0);						 
	xlsb = BMP085_RecvByte();	 
	BMP085_SendACK(1);	 //���һ��������Ҫ��NOACK

	BMP085_Stop();							 //ֹͣ�ź�
	
	up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
	
	return up;
}

//**************************************************************

//��ʼ��BMP085
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

// ��ʼ�����º���ѹ
// �������Ȳ����£��ڲ���ѹ��
// ����������Ҫ4.5ms
// ������ѹ��Ҫ4.5-25.5ms���ȣ�������ͷ�ļ�����OSS�����ã�OSS=0 4.5ms, OSS=1 7.5ms, OSS=2 13.5ms, OSS=3 25.5ms)
void bmp085Convert()
{
	long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	
	ut = bmp085ReadUT(); // ��ȡ�¶�
	up = bmp085ReadUP(); // ��ȡѹǿ
	
	//�����¶�
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	bmp085Temperature = (b5 + 8) >> 4;  // temperature * 0.1 ���϶�

	//������ѹ
	b6 = b5 - 4000;  //ע�⣺b5����ǰ������¶�ʱ�������һ���������������ѹǰ������¶�.
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
	bmp085Pressure = p + ((x1 + x2 + 3791) >> 4); // ��λ(Pa)  [1Pa=0.01hPa=0.01mbar]
}

// �õ����һ�β���������
// (����ֵ*0.1)���ɵõ����϶�
// ���磺��ǰ����Ϊ25.3���϶ȣ���ô����ֵΪ253
long bmp085GetTemperature()
{
	return bmp085Temperature;
}

// �õ����һ�β�������ѹ
// ��λΪPa  [1Pa=0.01hPa=0.01mbar]
long bmp085GetPressure()
{
	return bmp085Pressure;
}