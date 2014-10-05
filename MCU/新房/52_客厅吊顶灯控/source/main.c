/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
2位继电器模块*2


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年09月05日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

//STC11F04E 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"


// Node ID
#define NODE_ID 52

sbit RELAY_1 = P1^4;
sbit RELAY_2 = P1^5;
sbit RELAY_3 = P1^6;
sbit RELAY_4 = P1^7;

//开机延时 
//根据NodeID，进行约为(500*NodeID)毫秒的延时
//作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
//void initDelay(void)
//{
//	//4MHz Crystal, 1T STC11F04E
//    unsigned char a,b,c,d;
//    for(d=NODE_ID;d>0;d--)
//	    for(c=167;c>0;c--)
//	        for(b=171;b>0;b--)
//	            for(a=16;a>0;a--);
//}

//初始化继电器
void initRelays()
{
	RELAY_1=1;
	RELAY_2=1;
	RELAY_3=1;
	RELAY_4=1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 52}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr);
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
	unsigned char fromNodeID;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	fromNodeID = *(receivedData++); //发送者的ID
	
	RELAY_1 = (*(receivedData++)==0)?1:0;
	RELAY_2 = (*(receivedData++)==0)?1:0;
	RELAY_3 = (*(receivedData++)==0)?1:0;
	RELAY_4 = (*(receivedData++)==0)?1:0;
}
