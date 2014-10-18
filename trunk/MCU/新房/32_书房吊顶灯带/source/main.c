/***************************************************************************
【硬件信息】
单片机: STC11F04E, 1T, 内部RC振荡
其它：
继电器模块 2个


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年10月18日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/


#include<reg52.h>
#include "nrf24L01Node.h"

// Node ID
#define NODE_ID 83

sfr AUXR = 0x8E;

sbit OUT_RELAY=P1^6;  //继电器控制端 1-off, 0-on

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void initRelays()
{
	//初始化继电器
	OUT_RELAY = 1;
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 32, 231, 32}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


main()
{
	
	//初始化继电器
	initRelays();
		
	//初始化中断0
	initINT0();
	
	//初始化nrf
	nrf24L01Init();
	
	startRecv(); //开始接收
	
	while(1)
	{

	}
}

//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	
	receivedData = nrfGetReceivedData();
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 20: //执行命令，无需返回
			
			//控制继电器工作模式（0：常关，1：常开）
			OUT_RELAY = (*(receivedData+2)==0)?1:0;

		
		case 50: //请求立即报告状态
		
			break;
	
	}//end of switch
		
	
	//再次进入接收模式
	//nrfSetRxMode();
}