/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2011��09��25��  �Ƴ���  ��ʼ�汾
2013��10��14��  �Ƴ���  �޸���ʱ������ȥ����Delay.h������

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>     //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���
#include "ds18B20.h"

/*------------------------------------------------
                  ��������
------------------------------------------------*/
unsigned char readOneByte(void);
void writeOneByte(unsigned char writeByte);


// ��ʱ����
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
18b20��ʼ��
����1���ʼ���ɹ�
����0���ʼ��ʧ��
----------------*/
bit initDS18B20(void)
{
	bit returnData;
	
	DS18B20 = 1;    //DQ��λ
	ds18B20Delay50us();//������ʱ
	
	DS18B20 = 0;    //��Ƭ����DQ����
	ds18B20Delay600us(); //��ȷ��ʱ ���� 480us С��960us
	DS18B20 = 1;     //��������
	ds18B20Delay120us();  //15~60us �� ����60-240us�Ĵ�������
	returnData=~DS18B20; //���x=1���ʼ���ɹ�, x=0���ʼ��ʧ��

	ds18B20Delay50us();//������ʱ����

	
	return returnData;
}

/*------------
��ȡһ���ֽ�
-------------*/
unsigned char readOneByte(void)
{
	unsigned char i=0;
	unsigned char returnData = 0;
	for (i=8;i>0;i--)
	{
		DS18B20 = 0; // �������ź�
		returnData>>=1;
		DS18B20 = 1; // �������ź�
		if(DS18B20) returnData|=0x80;
		
		ds18B20Delay50us();
	}
	return(returnData);
}

/*-----------
д��һ���ֽ�
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


// ��ȡ�¶�

// ������ģʽ��0��1
// ģʽ0������readTemperature(0)ʵ��ȡ�õ����ϴζ�ȡʱ�Ĳ���¶�ֵ
// ģʽ1������readTemperature(1)��ȡ�õ�ǰ���¶�ֵ��������Ҫ1s������ŷ��ء�
// �����Ǹ����������¶�ֵ�� 

float readTemperature(bit mode)
{
	unsigned char tempH, tempL;
	float returnData;
	
	initDS18B20();
	writeOneByte( 0xCC ); // ����������кŵĲ���
	writeOneByte( 0x44 ); // �����¶�ת��
	
	if( mode )
	{
		ds18B20Delay1s(); //12λ�ֱ��ʣ���Ҫ750ms����
	}

	initDS18B20();
	writeOneByte( 0xCC ); //����������кŵĲ��� 
	writeOneByte( 0xBE ); //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
	
	tempL = readOneByte();   //��λ
	tempH = readOneByte();   //��λ

	
	if( tempH & 0x80 )
		returnData=(~( tempH * 256 + tempL) + 1) * (-0.0625);  //�������, �¶�ת�����Ѹ�λ��λ����Ӧ������ת��Ϊʵ�ʵ��¶� 
	else
		returnData=( tempH * 256 + tempL ) * 0.0625;   //�������
	
	return( returnData );
}
