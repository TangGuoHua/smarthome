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
	//ADC_CONTR &= ~ ADC_POWER; // 关闭电源，有利于降低功耗，但会多耗时降低速度。要求高速转换时，不关也可以
	
	// 低2位的值在ADC_LOW2里面，例如: SendData( ADC_LOW2 );  // send ADC low 2 bit result
	
	return ADC_DATA;  //return result
}


void Delay(unsigned int n)
{
	unsigned int x;
	while(n--)
	{
		x=3000;  //例子里面晶振是18.432Mhz  x=5000, 11.0592Mhz x=3000;
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
