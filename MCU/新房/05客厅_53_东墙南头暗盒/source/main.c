/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
10A*2继电器模块

【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2014年03月29日  黄长浩  初始版本


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
#define NODE_ID 53


sbit RELAY1 = P1^7;
sbit RELAY2 = P1^6; 


//开机延时 
//根据NodeID，进行约为(500*NodeID)毫秒的延时
//作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
void initDelay(void)
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=167;c>0;c--)
	        for(b=171;b>0;b--)
	            for(a=16;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}





//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 53}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr);
}





void main()
{
	//初始化继电器
	RELAY1 = 1;
	RELAY2 = 1;

	//初始化中断0
	initINT0();
	
	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	initDelay();
			
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
	
	//字节1，控制1号继电器工作模式（0：常关，1：常开）
	RELAY1 = (*(receivedData++)==0)?1:0;
	
	//字节2，控制2号继电器工作模式（0：常关，1：常开）
	RELAY2 = (*(receivedData++)==0)?1:0;
}
