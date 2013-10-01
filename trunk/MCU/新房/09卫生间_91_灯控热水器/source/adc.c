/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年10月01日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
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
	//ADC_CONTR &= ~ ADC_POWER; // 关闭电源，有利于降低功耗，但会多耗时降低速度。要求高速转换时，不关也可以
	
	// 低2位的值在ADC_LOW2里面，例如: SendData( ADC_LOW2 );  // send ADC low 2 bit result
	
	return ADC_DATA;  //return result
}


void adcDelay(unsigned int n)
{
	unsigned int x;
	while(n--)
	{
		x=1200;  //例子里面晶振是18.432Mhz  x=5000, 11.0592Mhz x=3000; 4Mhz x=1200
		while(x--);
	}
}


void initADC()
{
	//P1=P1M0=P1M1=0xff;  // set all P1 as Open-drain mode
	P1M0=P1M1=0x10; //将P1.4设置为开漏模式，P1其它口设为准双向模式，（即复位后默认模式）
	
	ADC_DATA = 0;  // clear previous result
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL;
	adcDelay(2);   //ADC power-on and delay
}
