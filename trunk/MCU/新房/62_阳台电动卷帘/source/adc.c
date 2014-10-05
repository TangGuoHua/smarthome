/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2013��10��01��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include "adc.h"

unsigned char getADCResult( unsigned char ch)
{
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ch|ADC_START;
	_nop_();  // must wait awhile before inquiry
	_nop_();
	_nop_();
	_nop_();
	while( ! (ADC_CONTR & ADC_FLAG) );  // wait for completion flag
	ADC_CONTR &= ~ ADC_FLAG;  // close ADC
	//ADC_CONTR &= ~ ADC_POWER; // �رյ�Դ�������ڽ��͹��ģ�������ʱ�����ٶȡ�Ҫ�����ת��ʱ������Ҳ����
	
	// ��2λ��ֵ��ADC_LOW2���棬����: SendData( ADC_LOW2 );  // send ADC low 2 bit result
	
	return ADC_DATA;  //return result
}


void adcDelay(unsigned int n)
{
	unsigned int x;
	while(n--)
	{
		x=1200;  //�������澧����18.432Mhz  x=5000, 11.0592Mhz x=3000; 4Mhz x=1200
		while(x--);
	}
}


void initADC()
{
	//P1=P1M0=P1M1=0xff;  // set all P1 as Open-drain mode
	P1M0=P1M1=0x10; //��P1.4����Ϊ��©ģʽ��P1��������Ϊ׼˫��ģʽ��������λ��Ĭ��ģʽ��
	
	ADC_DATA = 0;  // clear previous result
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL;
	adcDelay(2);   //ADC power-on and delay
}
