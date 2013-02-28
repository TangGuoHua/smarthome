/*
When        Who        Remarks
--------------------------------------
2011-NOV-13 Changhao   Initial version
*/

#include <reg51.h>
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


void Delay(unsigned int n)
{
	unsigned int x;
	while(n--)
	{
		x=3000;  //�������澧����18.432Mhz  x=5000, 11.0592Mhz x=3000;
		while(x--);
	}
}


void initADC()
{
	P1=P1M0=P1M1=0xff;  // set all P1 as Open-drain mode
	ADC_DATA = 0;  // clear previous result
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL;
	Delay(2);   //ADC power-on and delay
}
