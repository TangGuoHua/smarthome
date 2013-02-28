/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include <reg52.h>     //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���
#include "ds18B20.h"

/*------------------------------------------------
                  ��������
------------------------------------------------*/
unsigned char readOneByte(void);
void writeOneByte(unsigned char writeByte);

//��ʱ10us 4M���� STC12C5608AD 1T
void ds18B20Delay10us(void)   //��� 0us
{
    unsigned char a;
    for(a=7;a>0;a--);
}

//��ʱ100us 4M���� STC12C5608AD 1T
void ds18B20Delay100us(unsigned char b)   //��� 0us
{
    unsigned char a;
    for(;b>0;b--)
        for(a=98;a>0;a--);
}

// ��ʱ1MS 
void ds18B20DelayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M����
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
	ds18B20Delay10us();   //������ʱ
	
	DS18B20 = 0;    //��Ƭ����DQ����
	ds18B20Delay100us(6); //��ȷ��ʱ ���� 480us С��960us
	DS18B20 = 1;     //��������
	ds18B20Delay100us(1);  //15~60us �� ����60-240us�Ĵ�������
	returnData=~DS18B20; //���x=1���ʼ���ɹ�, x=0���ʼ��ʧ��
	
	ds18B20Delay10us();  //������ʱ����
	ds18B20Delay10us();
	ds18B20Delay10us();
	
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
д��һ���ֽ�
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


// ��ȡ�¶�
// ������ģʽ��0��1
// ģʽ0������readTemperature(0)ʵ��ȡ�õ����ϴζ�ȡʱ�Ĳ���¶�ֵ
// ģʽ1������readTemperature(1)��ȡ�õ�ǰ���¶�ֵ��������ҪԼ800+ms�����ŷ��ء�
// �����Ǹ����������¶�ֵ�� 
float getTemperature(bit mode)
{
	unsigned char tempH, tempL;
	float returnData;
	
	initDS18B20();
	writeOneByte( 0xCC ); // ����������кŵĲ���
	writeOneByte( 0x44 ); // �����¶�ת��
		
	if( mode )
	{
		ds18B20DelayMS(800); //12λ�ֱ��ʣ���Ҫ750ms����
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
