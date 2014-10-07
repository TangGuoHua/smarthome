/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
奥松AM2321温湿度传感器

【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2014年01月26日  黄长浩  初始版本
2014年01月27日  黄长浩  增加initDelay()
2014年02月11日  黄长浩  修正AM2321读数全零的bug
2014年10月07日  黄长浩  升级NRF24L01+驱动

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

//STC11F04E 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"
#include "am2321.h"

// Node ID
#define NODE_ID 54

sfr AUXR   = 0x8E;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerCounter1s = 0;

//
////开机延时 
////根据NodeID，进行约为500*NodeID毫秒的延时
////作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
//void initDelay(void)
//{
//	//4MHz Crystal, 1T STC11F04E
//    unsigned char a,b,c,d;
//    for(d=NODE_ID;d>0;d--)
//	    for(c=167;c>0;c--)
//	        for(b=171;b>0;b--)
//	            for(a=16;a>0;a--);
//}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}



// 初始化Timer0
// 每10ms触发中断1一次
// （STC 1T @4MHz 外部晶振）
void initTimer0(void)
{
	AUXR = AUXR|0x80;  //设置T0为1T模式
	TMOD = 0x01;
	TH0 = 0x63;
	TL0 = 0xC0;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}




//向主机发送当前的温度和继电器状态
void sendDataToHost( unsigned char readAm2321Result )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3字节地址
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = readAm2321Result;
	sendData[2] = getAM2321Data(0);
	sendData[3] = getAM2321Data(1);
	sendData[4] = getAM2321Data(2);
	sendData[5] = getAM2321Data(3);
	sendData[6] = getAM2321Data(4);
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);
	
	//24L01开始接收数据
	//startRecv(); 
}



void main()
{
	unsigned char readAm2321Result = 0;
	
	//初始化中断0
	initINT0();
	
	//init AM2321
	initAM2321();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	//startRecv(); 
	
	//初始化延时
	//initDelay();
	
	// 初始化Timer0
	initTimer0();	
			
	while(1)
	{
		if(sendDataNow)
		{
			sendDataNow=0;
			//触发AM3231测量，并读取上一次测量的温湿度值
			readAm2321Result = readAM2321( 2 );

			//向主机发送数据
			sendDataToHost( readAm2321Result );	

		}
	}
}



////外部中断0处理程序
////用于处理来自24L01的中断
//void interrupt24L01(void) interrupt 0
//{
//	unsigned char * receivedData;
//	
//	//获取接收到的数据
//	receivedData = nrfGetReceivedData();
//	
//	//字节1，控制1号继电器工作模式（0：常关，1：常开，2：延时关）
//	relay1Mode = *(receivedData++);
//	if( relay1Mode == 0 ) RELAY_1=1;
//	else if( relay1Mode == 1 ) RELAY_1=0;
//	else if( relay1Mode == 2 )
//	{
//		relay1DelayOffTimerCount = 0; //延时关计数清零
//	}
//
//	
//	//字节2，控制2号继电器的工作模式 （0：常关，1：常开，2：温控）
//	relay2Mode = *(receivedData++);
//	if( relay2Mode == 0 ) RELAY_2=1;
//	else if( relay2Mode == 1 ) RELAY_2=0;
//	
//	//字节3，温度上限 10倍数
//	tUpper = *(receivedData++);
//	
//	//字节4，温度下限 10倍数
//	tLower = *(receivedData++);
//}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100个10ms，即1秒
	{
		timerCounter10ms=0;
		
		//向Pi发送数据
		if( ++timerCounter1s == 10 ) //每600秒一次
		{
			timerCounter1s = 0;
			sendDataNow = 1;
		}
	}
}