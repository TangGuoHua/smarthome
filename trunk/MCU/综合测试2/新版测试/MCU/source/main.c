//STC12C5616AD 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"
#include <stdlib.h>  

sbit LED1 = P1^4;
sbit LED2 = P1^5;

volatile unsigned char replyFlag = 0;
volatile unsigned char channel;
volatile unsigned char qty;

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}

void delay500ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=46;c>0;c--)
        for(b=152;b>0;b--)
            for(a=70;a>0;a--);
}

void delay100us(void)   //误差 0us
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=97;a>0;a--);
}

void delay5s(void)   //11.0592Mhz
{
    unsigned char a,b,c,n;
    for(c=249;c>0;c--)
        for(b=219;b>0;b--)
            for(a=252;a>0;a--);
    for(n=32;n>0;n--);

}

//void delay10s(void) 
//{
//
//	//4Mhz Crystal, 1T STC11F04E
//    unsigned char a,b,c;
//    for(c=191;c>0;c--)
//        for(b=209;b>0;b--)
//            for(a=249;a>0;a--);
//
//}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97,83,175,231,231}; //本节点的接收地址
	nrfSetRxMode( 125, 3, myAddr);
}




void replyData( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 70, 132, 231, 231}; //Pi, 3字节地址
	
	//上一帧数据发送时，re-transmit的次数。
	//0:表示一次发送成功，没有re-transmit
	//1-15:表示重发了1－15次成功
	//100: 本帧为发送的第一帧数据，所以没有上一次发送的re-transmit计数
	//255: 发送失败
	unsigned char lastReTxn=100; 
	
	unsigned char cntReTxn0=0; //一次发送成功，重发0次，的帧数
	unsigned char cntReTxn5=0; //重发1－5次成功的帧数
	unsigned char cntReTxn10=0;//重发6－10次成功的帧数
	unsigned char cntReTxn15=0;//重发11－15次成功的帧数
	unsigned char cntFailed=0;//发送失败的帧数
	
	unsigned char c;
	
	
	sendData[0] = 210;//Node ID
	sendData[1] = 2;// reply to Function 1
	
	for( c=1;c<=(qty+1);c++ )
	{
		
		sendData[2] = c;
		sendData[3] = lastReTxn;
		sendData[4] = cntReTxn0;
		sendData[5] = cntReTxn5;
		sendData[6] = cntReTxn10;
		sendData[7] = cntReTxn15;
		sendData[8] = cntFailed;
	
		lastReTxn = nrfSendData(channel, 5, toAddr, 16, sendData);
		
		if( lastReTxn == 0 )
			cntReTxn0++;
		else if( 1<=lastReTxn && lastReTxn<=5 )
			cntReTxn5++;
		else if( 6<=lastReTxn && lastReTxn<=10 )
			cntReTxn10++;
		else if( 11<=lastReTxn && lastReTxn<=15 )
			cntReTxn15++;
		else if( lastReTxn == 255 )
			cntFailed++;

	}
	
	 
}

void sendData()
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231};
	
	sendData[0] = 251;
	sendData[1] = 1;
	sendData[2] = 2;
	sendData[3] = 3;
	sendData[4] = 4;
	sendData[5] = 5;
	
	nrfSendData(96, 5, toAddr, 16, sendData);

}


void main()
{

	

	//delay500ms();
	
	//RELAY=1; //switch off relay
	
	//初始化中断0
	//initINT0();
	
	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	//startRecv(); 
	

	while(1)
	{	
		sendData();
		delay5s();
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	channel = *(receivedData+2);
	qty = *(receivedData+3);
	
	replyFlag = 1;

}
