/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
继电器模块 2个


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年11月08日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"

sbit RELAY_LIGHT_E = P1^3; //灯控继电器 东
sbit RELAY_LIGHT_S = P3^5; //灯控继电器 南
sbit RELAY_LIGHT_W = P1^7; //灯控继电器 西
sbit RELAY_LIGHT_N = P1^6; //灯控继电器 北

//初始化INT0 
//nrf24L01接收需要用到
void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 41}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr); //接收92频道，3字节地址
}



//初始化继电器
void initRelays()
{
	RELAY_LIGHT_E=1;
	RELAY_LIGHT_S=1;
	RELAY_LIGHT_W=1;
	RELAY_LIGHT_N=1;
}

void main()
{
	//初始化继电器
	initRelays();
	
	//初始化INT0 
	initINT0();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	
	while(1)
	{
	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	RELAY_LIGHT_E = (*(receivedData+0))==1?0:1 ;
	RELAY_LIGHT_S = (*(receivedData+1))==1?0:1 ;
	RELAY_LIGHT_W = (*(receivedData+2))==1?0:1 ;
	RELAY_LIGHT_N = (*(receivedData+3))==1?0:1 ;

}