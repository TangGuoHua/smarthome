/*****************************************
1T STC11F04E 3.6864MHz
PI与单片机串口通信

Revision History
Date         Author      Remarks
-----------------------------------------------------------
2013-FEB-27  Changhao    去掉了blinkLED （但还没有烧入单片机）
                         （准备将串口速率提高到115200bps，Pi的程序也要相应改动）

*****************************************/

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
sbit LED1=P1^7;
sbit LED2=P1^6;

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
	unsigned char sendAddr[ADDRESS_WIDTH]; //= { 0x61, 0x53, 0x95 }; //从节点地址
	unsigned char sendDataLen;

	sendRFChannel = *(txData+2);
	
	sendAddr[0] = *(txData+3);
	sendAddr[1] = *(txData+4);
	sendAddr[2] = *(txData+5);
	
	sendDataLen = *(txData+6);
	
	nrfSendData( sendRFChannel, ADDRESS_WIDTH, sendAddr, sendDataLen, (txData+7) );//发送	
}


//void delay1s(void)   //误差 -0.000000000023us
//{
//    unsigned char a,b,c;
//    for(c=23;c>0;c--)
//        for(b=216;b>0;b--)
//            for(a=184;a>0;a--);
//}


//void delay100ms(void)   //误差 -0.000000000002us
//{
//    unsigned char a,b,c;
//    for(c=131;c>0;c--)
//        for(b=156;b>0;b--)
//            for(a=3;a>0;a--);
//}

void delay50ms(void)   //误差 -0.000000000001us
{
    unsigned char a,b;
    for(b=221;b>0;b--)
        for(a=207;a>0;a--);
}

//void blinkLED( unsigned char c)
//{
//	if(c==1)
//	{
//		LED1=0;
//		delay50ms();
//		LED1=1;
//	}
//	else
//	{
//		LED2=0;
//		delay50ms();
//		LED2=1;
//	}
//}


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
	
	nrfSetRxMode(); //开始接收
		
	while(1)
	{
		sendDataToNode( serialPortReceive() );
		//blinkLED(2);

		nrfSetRxMode();//设置为接收模式

	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	
	//blinkLED(1);  //blink GREEN LED
	
	//读取接收到的数据
	receivedData = nrfGetReceivedData();
	
	// 将数据组帧后通过串口发给主机
	serialPortSendFrame( *receivedData++, *receivedData++, receivedData );

	//再次进入接收模式
	nrfSetRxMode();

}
