/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
继电器模块


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年07月30日  黄长浩  初始版本


【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
//#include "adc.h"
//#include "stcEEPROM.h"
//#include "ds18B20.h"

// Node ID
#define NODE_ID 62

sfr AUXR   = 0x8E;

//sbit LDR = P1^7;  //光敏电阻 （10K上拉）


sbit RELAY_POWER = P2^1; //继电器1，控制电源
sbit RELAY_DIRECTION = P2^0; //继电器2，控制方向，常闭-上升，常开-下降

sbit K1=P1^7; //升
sbit K2=P1^6; //降

unsigned char curtainMode = 1; //1:手动，2：自动
unsigned char curtainOpenPercent;
bit motorDirection = 0; //马达运动方向，1：上升（开卷帘），0：下降（关卷帘）
bit motorOn = 0; //马达开关, 1：开， 0：关

unsigned char timerCounter10ms = 0;
unsigned char timerCounter1s = 0;
//unsigned int timerSendData = 0;

// Flag for sending data to Pi
//bit sendDataNow = 0;


void delay200ms()
{
    unsigned char a,b,c;
    for(c=29;c>0;c--)
        for(b=70;b>0;b--)
            for(a=97;a>0;a--);
}

void stopMotor()
{
	//停马达电源
	RELAY_POWER = 1;
	
	TR0=0;
	
	delay200ms();
	
	//方向选择继电器断电
	RELAY_DIRECTION = 1;
}

void startMotor()
{
	//设定方向
	RELAY_DIRECTION = motorDirection;
	
	delay200ms();
	
	//通电
	RELAY_POWER = 0;
	
	TR0 = 1;
}

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




//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 62}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr); //接收92频道，3字节地址
}
//
//
////返回当前亮度值
//unsigned char getBrightness()
//{
//	return (255-getADCResult(7));
//}
//
//
//
////将开灯阈值存入片内eeprom
//void saveLightOnThreshold( unsigned char threshold1, unsigned char threshold2 )
//{
//	eepromEraseSector( 0x0000 );
//	eepromWrite( 0x0000, threshold1 );
//	eepromWrite( 0x0002, threshold2 );
//}
//
//////从片内eeprom取得开灯阈值
////unsigned char getLightOnThreshold()
////{
////	return eepromRead(0x0000);
////}
//
//
void sendDataToHost( unsigned char a, unsigned char b, unsigned char c, unsigned char d )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96频道，3字节地址，接收16字节
	unsigned char tmp;

		
	sendData[0] = NODE_ID;//Node ID

	sendData[1] = a;
	sendData[2] = b;
	sendData[3] = c; 
	sendData[4] = d; 


	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//向Pi发送, 96频道，3字节地址，16字节数据
	
	//24L01开始接收数据
	startRecv(); 
}


// 初始化Timer0
void initTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    //TR0 = 1;
}

void initRelays()
{
	RELAY_POWER=1;
	RELAY_DIRECTION=1;
}

void main()
{


	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//初始化中断0
	initINT0();
    
	//初始化继电器
	initRelays();
	
	//初始化DS18B20
	//initDS18B20();
	
	//初始化ADC
	//initADC();
	
	//取得开灯阈值
	//light1OnThreshold = eepromRead(0x0000);
	//light2OnThreshold = eepromRead(0x0002);

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	//initDelay();
	
	//初始化timer0
	initTimer0();
	
	
	
	while(1)
	{
//		if( motorOn && (RELAY_POWER==1) ) //说明电机当前不在工作
//		{
//			startMotor();
//		}

		if( K1==0 && RELAY_POWER == 1 )
		{
			timerCounter10ms = 0;
			timerCounter1s = 0;
			motorDirection=1;
			//TR0 = 1;
			startMotor();
			
			sendDataToHost( 1, 100, 0, 0 );
			
			delay200ms();
			delay200ms();
			
		}
		else if( K1==0 && RELAY_POWER == 0 )
		{
			//TR0 = 0;
			stopMotor();
			
			sendDataToHost( 1, 200, timerCounter1s, timerCounter10ms );
			delay200ms();
			delay200ms();
		}

		if( K2==0 && RELAY_POWER == 1 )
		{
			timerCounter10ms = 0;
			timerCounter1s = 0;
			motorDirection=0;
			//TR0 = 1;
			startMotor();
			
			sendDataToHost( 0, 100, 0, 0 );
			
			delay200ms();
			delay200ms();
			
		}
		else if( K2==0 && RELAY_POWER == 0 )
		{
			//TR0 = 0;
			stopMotor();
			
			sendDataToHost( 0, 200, timerCounter1s, timerCounter10ms );
			delay200ms();
			delay200ms();
		}
	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char rMode, rOpenPercent;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): 发送者NodeID
	// *(receivedData+1): 功能号
	// *(receivedData+2): 卷帘工作模式
	// *(receivedData+3): 开度

	
	//开灯阈值
	rMode = *(receivedData+0);
	rOpenPercent = *(receivedData+1);
	
	if( rMode == 0)
	{
		stopMotor();
	}
	else if( rMode == 1) //升
	{
		motorDirection=1;
		startMotor();
	}
	else if( rMode ==2 ) //降
	{
		motorDirection=0;
		startMotor();
	}
	
	
//	if( rOpenPercent != curtainOpenPercent )
//	{
//		if( rOpenPercent > curtainOpenPercent )
//		{
//			//开窗帘
//			motorDirection = 1;
//			motorOn = 1;
//		}
//		else
//		{
//			//关窗帘
//			motorDirection = 0;
//			motorOn = 1;
//		}
//	}
	

	

}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100个10ms，即1秒
	{
		timerCounter10ms=0;
		timerCounter1s++;
		

		

	}
}
