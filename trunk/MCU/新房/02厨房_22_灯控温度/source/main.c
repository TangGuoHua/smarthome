/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
人体红外PIR探头
继电器模块


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年09月26日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include <intrins.h>

//红外接收头
sbit PIR=P1^3;

sbit RELAY1=P1^5;
sbit RELAY2=P1^4;

void main(void)
{
	RELAY1=1;
	RELAY2=1;
	
	while(1)
	{
		RELAY1=~PIR;
		RELAY2=~PIR;
	}
}
