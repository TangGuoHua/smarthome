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

#ifndef __ADC_H__
#define __ADC_H__

sfr ADC_CONTR = 0xC5;  // ADC control register
sfr ADC_DATA  = 0xC6;  // ADC high 8-bit result register
sfr ADC_LOW2  = 0xBE;  // ADC low 2 bit result register
sfr P1M0      = 0x91;  // P1 mode control register0
sfr P1M1      = 0x92;  // P1 mode control registe1

#define ADC_POWER    0x80  // ADC power control bit
#define ADC_FLAG     0x10  // ADC completion flag
#define ADC_START    0x08  // ADC start control bit
#define ADC_SPEEDLL  0x00  // 1080 clocks per convert
#define ADC_SPEEDL   0x20  // 810 clocks per convert
#define ADC_SPEEDH   0x40  // 540 clocks per convert
#define ADC_SPEEDHH  0x60  // 270 clocks per convert

void initADC();
unsigned char getADCResult( unsigned char ch );

#endif