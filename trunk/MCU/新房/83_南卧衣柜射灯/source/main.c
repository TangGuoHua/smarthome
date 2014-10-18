/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
继电器模块10A*2路


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

//STC11F04E 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"

// Node ID
#define NODE_ID 83

sfr AUXR   = 0x8E;

sbit RELAY1 = P1^7;
sbit RELAY2 = P1^6; 

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char functionNum = 0;
volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerCounter1s = 0;

//延时5秒 STC11F04E 4MHz
void delay5s(void)
{
    unsigned char a,b,c;
    for(c=191;c>0;c--)
        for(b=189;b>0;b--)
            for(a=137;a>0;a--);
}


//开机延时 
//作用是避免所有节点同时上电，若都按相同的时间间隔发送数据造成的通讯碰撞
void initDelay(void)
{
	//120秒
    unsigned n;
    for(n=0;n<24;n++)
	{
		delay5s();
	}

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


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void initRelays()
{
	//初始化继电器
	RELAY1 = 1;
	RELAY2 = 1;
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 83, 231, 83}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//向主机发送当前的温度和继电器状态
void sendDataToHost()
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3字节地址
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = functionNum;// Function Number, 1-regular status update, 51-respond to 50
	sendData[2] = ~RELAY1;
	sendData[3] = ~RELAY2;
	
	TR0 = 0;//Pause timer0
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);
	
	//24L01开始接收数据
	startRecv();
	TR0 = 1;//Resume timer0
	
}


void main()
{
	unsigned char readAm2321Result = 0;
	
	//初始化继电器
	initRelays();
	
	//初始化中断0
	initINT0();
	
	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	initDelay();
	
	// 初始化Timer0
	initTimer0();	
			
	while(1)
	{
		if(sendDataNow)
		{
			sendDataNow=0;
			
			//向主机发送数据
			sendDataToHost();	

		}
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	TR0 = 0;//Pause timer0

	
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 20: //执行命令，无需返回
			
			//字节1，控制1号继电器工作模式（0：常关，1：常开）
			RELAY1 = (*(receivedData+2)==0)?1:0;
			
			//字节2，控制2号继电器工作模式（0：常关，1：常开）
			RELAY2 = (*(receivedData+3)==0)?1:0;		
			break;
		

		
		case 50: //请求立即报告状态
			functionNum = 51;//回报50
			timerCounter1s = 0;//计时重新开始
			sendDataNow = 1;		
			break;
	
	}//end of switch
	
	
	TR0 = 1;//Resume timer0

}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100个10ms，即1秒
	{
		timerCounter10ms=0;
		
		//向Pi发送数据
		if( ++timerCounter1s == 600 ) //每600秒一次
		{
			timerCounter1s = 0;
			functionNum = 1;//定时状态报告
			sendDataNow = 1;
		}
	}
}