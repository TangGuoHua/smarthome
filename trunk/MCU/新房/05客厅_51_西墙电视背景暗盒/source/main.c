/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：

8位继电器模块 

因为控制器板子端口不够（只有7个可用口）
2号继电器接收音机，需要一直通电，故2号继电器不与控制器连接，使用它的常闭触点


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年06月29日  黄长浩  初始版本
2014年10月04日  黄长浩  升级nrf24L01+驱动
                        修改2号继电器为不受控
						增加向Pi定时状态报告

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

//STC11F04E 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"


sfr AUXR   = 0x8E;

// Node ID
#define NODE_ID 51

sbit RELAY_1 = P1^3;
//sbit RELAY_2 = P1^3;
sbit RELAY_3 = P3^5;
sbit RELAY_4 = P1^5;
sbit RELAY_5 = P1^4;
sbit RELAY_6 = P1^7;
sbit RELAY_7 = P1^6;
sbit RELAY_8 = P3^7;


volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerSendData = 0;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;

//开机延时 
//根据NodeID，进行约为(NodeID*2)秒的延时
//作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
void initDelay(void)
{
	//4MHz Crystal, 1T
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=252;c>0;c--)
	        for(b=230;b>0;b--)
	            for(a=33;a>0;a--);
}

//初始化继电器
void initRelays()
{
	RELAY_1=1;
	//RELAY_2=1;
	RELAY_3=1;
	RELAY_4=1;
	RELAY_5=1;
	RELAY_6=1;
	RELAY_7=1;
	RELAY_8=1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


// 初始化Timer0
void initTimer0(void)
{
	AUXR = AUXR|0x80;  // T0, 1T Mode
	TMOD = 0x01;
	TH0 = 0x63;
	TL0 = 0xC0;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 51, 231, 51}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//发送数据给Pi
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update

	sendData[2] = ~RELAY_1;
	sendData[3] = 0; //RELAY_2不用，跳过
	sendData[4] = ~RELAY_3;
	sendData[5] = ~RELAY_4;
	sendData[6] = ~RELAY_5;
	sendData[7] = ~RELAY_6;
	sendData[8] = ~RELAY_7;
	sendData[9] = ~RELAY_8;
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96频道，5字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv(); 
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
	
	//初始化延时
	initDelay();
	
	//初始化timer0
	initTimer0();
	
	
	while(1)
	{
		//Send data to Pi
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();
		}
			
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
	receivedData++; // Relay_2 不用，跳过
	RELAY_3 = (*(receivedData++)==0)?1:0;
	RELAY_4 = (*(receivedData++)==0)?1:0;
	RELAY_5 = (*(receivedData++)==0)?1:0;
	RELAY_6 = (*(receivedData++)==0)?1:0;
	RELAY_7 = (*(receivedData++)==0)?1:0;
	RELAY_8 = (*(receivedData++)==0)?1:0;

}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100个10ms，即1秒
	{
		timerCounter10ms=0;
		
		if( ++timerSendData == 600 ) //每600秒向Pi发送一次数据
		{
			timerSendData = 0;
			sendDataNow = 1;
		}
		

	}
}