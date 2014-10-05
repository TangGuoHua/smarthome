/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2011年10月06日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#ifndef __STCEEPROM_H__
#define __STCEEPROM_H__

//#define STC_EEPROM_START_ADDR 0x0000  //STC12C5616AD片内EEPROM起始地址

unsigned char eepromRead( unsigned int address );
void eepromWrite( unsigned int address, unsigned char writeData );
void eepromEraseSector( unsigned int address );

#endif
