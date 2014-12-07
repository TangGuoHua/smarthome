/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
2位继电器模块*2
PIR模块


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年09月05日  黄长浩  初始版本
2014年12月07日  黄长浩  升级NRF驱动
                        增加PIR探头
						增加eeprom存值

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

//STC11F04E 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"
#include "stcEEPROM.h"

sfr AUXR = 0x8E;

// Node ID
#define NODE_ID 52

sbit PIR = P3^7;
sbit RELAY_1 = P1^4; //东
sbit RELAY_2 = P1^5; //西
sbit RELAY_3 = P1^6; //南
sbit RELAY_4 = P1^7; //北


volatile bit gPeopleIn = 0; //有人为1，无人为0

//PIR延时，用于判断是否无人，单位：分钟
//即，当PIR连续无人这么多分钟后，才判断客厅无人
volatile unsigned char gPIRDelay = 10; 


volatile unsigned char gTimerCounter10ms = 0;
volatile unsigned int gCounterRegularStatusReport = 0; //定时状态报告计时器
volatile unsigned int gCounterNoPeopleSeconds = 0; //无人的时间，单位：秒

volatile bit gNoPeopleCounterStart = 0; // 1:计时，没有人的时间， 0：不计时

volatile bit gSendDataNow=0; //向主机发送数据的标志位
volatile unsigned char gFuncNum=0; //功能号

//延时5秒 STC11F04E 4MHz
//（给下面的开机延时函数用）
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
	//40秒
    unsigned n;
    for(n=0;n<8;n++)
	{
		delay5s();
	}

}


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


void initTimer0(void)
{
	TMOD = 0x01;
	TH0 = 0x10;
	TL0 = 0x30;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 231, 231, 52}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//发送数据给Pi
void sendDataToHost()
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = gFuncNum; //Function Number

	sendData[2] = ~RELAY_1;
	sendData[3] = ~RELAY_2;
	sendData[4] = ~RELAY_3;
	sendData[5] = ~RELAY_4;
	sendData[6] = PIR;
	sendData[7] = gPeopleIn;
	sendData[8] = gPIRDelay;
	
	TR0=0;
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96频道，5字节地址，接收16字节
	TR0=1;
	
	//24L01开始接收数据
	startRecv(); 
}


//将PIR无人判断延时分钟数存入片内eeprom
void savePIRDelayMinute( unsigned char val )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, val );
}


void main()
{

	bit thisPIR=0;
	bit lastPIR=0;
	
	bit thisPeopleIn=0;
	bit lastPeopleIn=0;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode	
	
	//初始化继电器
	initRelays();
	
	//初始化INT0 
	initINT0();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv();
	
	//初始延时
	initDelay();
	
	//取得PIR无人判断延时分钟数
	gPIRDelay = eepromRead(0x0000);
	
	//初始化Timer0
	initTimer0();
	
	while(1)
	{
		thisPIR = PIR;
		
		if( thisPIR != lastPIR )
		{
			//有变化
			
			if( thisPIR )
			{
				//PIR有人
				gNoPeopleCounterStart = 0; //停止计时
				gCounterNoPeopleSeconds = 0;
				
				if( !gPeopleIn )
				{
					//无人->有人
					gPeopleIn = 1;
					
					gFuncNum = 2;//发送事件报告
					gSendDataNow = 1;

				}
			}
			else
			{
				//有人->无人
				gNoPeopleCounterStart = 1; //开始计时
				gCounterNoPeopleSeconds = 0;
				
			}
			
			lastPIR = thisPIR;
		}
		
		//发送数据给主机
		if(gSendDataNow)
		{
			sendDataToHost();
			gSendDataNow = 0;
			gCounterRegularStatusReport=0;//定时状态报告也重新开始
		}

	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	TR0=0;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	TR0=1;
		
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 22: //执行命令，并返回
			gFuncNum = 23;
			gSendDataNow = 1;
					
		case 20: //执行命令，无需返回
			RELAY_1 = (*(receivedData+2)==0)?1:0;
			RELAY_2 = (*(receivedData+3)==0)?1:0;
			RELAY_3 = (*(receivedData+4)==0)?1:0;
			RELAY_4 = (*(receivedData+5)==0)?1:0;
			if( gPIRDelay != *(receivedData+6))
			{
				gPIRDelay = *(receivedData+6);
				savePIRDelayMinute( gPIRDelay );
			}
			break;

		case 50: //请求立即报告状态
			//回报50, functionNum = 51;
			gFuncNum = 51;
			gSendDataNow = 1;
			gCounterRegularStatusReport=0;
			break;
	
	}//end of switch
}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++gTimerCounter10ms == 100 ) //100个10ms，即1秒
	{
		gTimerCounter10ms=0;
		
		//定时状态报告
		if( ++gCounterRegularStatusReport == 600 ) //每600秒向Pi发送一次数据
		{
			gCounterRegularStatusReport = 0;
			gFuncNum = 1; //发送定时状态报告
			gSendDataNow = 1;
		}
		
		//PIR无人计时
		if( gNoPeopleCounterStart ) 
		{
			if( ++gCounterNoPeopleSeconds >= gPIRDelay*60 )
			{
				//无人的时间够长了，真的判断为无人了
				
				gNoPeopleCounterStart = 0; //停止计时
				gCounterNoPeopleSeconds = 0; //计数器清零
				gPeopleIn = 0; //判断为无人
				
				gFuncNum = 2;//发送事件报告
				gSendDataNow = 1;			
			}
		}
		

	}
}
