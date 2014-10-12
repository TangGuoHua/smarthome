/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年01月26日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#ifndef __AM2321_H__
#define __AM2321_H__

//*********************
//**** IO口定义区 *****
//*********************
sbit  AM2321_SDA = P3^7;


// 函数定义
void initAM2321(void);
unsigned char readAM2321( unsigned char mode);
unsigned char getAM2321Data( unsigned char index );
#endif
