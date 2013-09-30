/********************************************************
1T STC11F04E 3.6864MHz
PI与单片机串口通信

作者保留一切权利。如有问题请联系huangchanghao@gmail.com

All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

Revision History
Date         Author      Remarks
-----------------------------------------------------------
2013-FEB-27  Changhao    去掉了blinkLED （但还没有烧入单片机）
2013-SEP-30  Changhao    串口波特率提高到115200bps
                         修改通讯协议：本主机节点固定每帧接收16字节，其中1个字节的NodeID，15个字节的数据。

**********************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "serialPort.c"

//Header0和Header1设计为一样的值，可以增强系统的容错性
//具体请参见上位机的MCUCommunicator类里面的注释

//MCU to PC
//BA9876543210
#define MCU2PC_FRAME_HEADER0 0xba
#define MCU2PC_FRAME_HEADER1 0xba
#define MCU2PC_FRAME_HEADER2 0x98
#define MCU2PC_FRAME_HEADER3 0x76

#define MCU2PC_FRAME_TAILER0 0x54
#define MCU2PC_FRAME_TAILER1 0x32
#define MCU2PC_FRAME_TAILER2 0x10


//sfr AUXR   = 0x8E;
sbit LED1=P1^7;  //Blue LED
sbit LED2=P1^6;  //Red LED

//初始化中断0，给NRF24L01用
void initINT0(void)
{
	EA=1; //打开总中断
	EX0=1; // Enable int0 interrupt.
}

// nrf24L01 send
void sendDataToNode( unsigned char* txData )
{
	//byte txData[2];
	unsigned char sendRFChannel;
	unsigned char sendAddr[ADDRESS_WIDTH];
	unsigned char sendDataLen;

	sendRFChannel = *(txData);
	
	sendAddr[0] = *(txData+1);
	sendAddr[1] = *(txData+2);
	sendAddr[2] = *(txData+3);
	
	sendDataLen = *(txData+4);
	
	nrfSendData( sendRFChannel, ADDRESS_WIDTH, sendAddr, sendDataLen, (txData+5) );//发送	
}


void delay50ms(void)   //误差 -0.000000000001us
{
    unsigned char a,b;
    for(b=221;b>0;b--)
        for(a=207;a>0;a--);
}

void blinkLED( unsigned char c)
{
	if(c==1)
	{
		LED1=0;
		delay50ms();
		LED1=1;
	}
	else
	{
		LED2=0;
		delay50ms();
		LED2=1;
	}
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {53, 69, 149 }; //本节点的接收地址
	nrfSetRxMode( 96, 3, myAddr);  //96频道，3字节宽度的地址
}

void main(void)
{
	
	delay50ms(); // 系统上电后稳定一下
	delay50ms();
	
	LED1=1;
	LED2=1;
	
    //AUXR = AUXR|0x40;  // T1, 1T Mode
    serialPortInit();

	//初始化中断0
	initINT0();
	
	nrf24L01Init();
	
	startRecv(); //开始接收
		
	while(1)
	{
		sendDataToNode( serialPortReceive() );
		blinkLED(2);  //blink RED LED

		startRecv();//设置为接收模式

	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	
	blinkLED(1);  //blink Blue LED
	
	//读取接收到的数据
	receivedData = nrfGetReceivedData();
	
	// 将数据组帧后通过串口发给主机
	serialPortSendFrame( *receivedData++, receivedData );

	//再次进入接收模式
	//startRecv();
}
