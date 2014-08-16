/***********************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
继电器模块


【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年07月30日  黄长浩  初始版本
2014年08月01日  黄长浩  实现指定覆盖度，消除累积误差


【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
//#include "adc.h"
//#include "stcEEPROM.h"
//#include "ds18B20.h"

sfr AUXR   = 0x8E;

// Node ID
#define NODE_ID 62

/*
行程控制说明:
系统通过控制电机运行时间（即ticks计数）来控制窗帘的位置。
系统设定timer0每10ms中断一次，产生一个tick，通过数ticks就可以知道运行时间。
通过实验，卷帘从最上到最下（下行）运行约41.5秒，上行全程运行约40.5秒。
在计算的时候，上下行程都按全程41.5秒计算。每10ms一个tick，则全程4150个ticks。

考虑到使用unsigned int来计数ticks，为避免出现0及负数，我们以500做为顶位置计数，4650最为最底位置计数

最大行程时间为 4650-500=4150 ticks， 即4150*10=41500ms=41.5s

当用户选择全开（0%覆盖）或者全关（100%覆盖）时，系统将多运行300*10ms，以消除累计误差
各种位置限制值如下图所示：

              底                   顶
（最下） |_____|____________________|_____|___| （最上）
       4950   4650                 500   200  0
         ^                                ^
  底校正余量                              顶校正余量
  
*/
//#define MAX_TICKS 4150 //最大运行时间（10ms计数值，即，最大运行时间为 MAX_TICKS*10毫秒 ）
#define POS_BOTTOM 4650
#define POS_TOP 500
#define CALIBRATION_LIMIT_TOP 200
#define CALIBRATION_LIMIT_BOTTOM 4950

//马达运动方向
//1：上升（开卷帘），0：下降（关卷帘）
#define DIRECTION_UP 1
#define DIRECTION_DOWN 0


//sbit LDR = P1^7;  //光敏电阻 （10K上拉）

sbit RELAY_POWER = P1^3; //继电器1，控制电源
sbit RELAY_DIRECTION = P3^5; //继电器2，控制方向，常闭-上升，常开-下降

//sbit RELAY_POWER = P2^1; //继电器1，控制电源
//sbit RELAY_DIRECTION = P2^0; //继电器2，控制方向，常闭-上升，常开-下降

//unsigned char curtainMode = 1; //1:手动，2：自动
unsigned char curtainCoverPercent; //卷帘覆盖度百分比

//马达运动方向
bit motorDirection = DIRECTION_UP; 


//卷帘当前位置 10ms ticks
unsigned int currentPosTicks = 0;

//目标位置 10ms ticks
unsigned int targetPosTicks = 0;

//继电器延时
void delayRelay()
{
	//4Mhz, 1T, 约300ms
    unsigned char a,b,c;
    for(c=244;c>0;c--)
        for(b=8;b>0;b--)
            for(a=152;a>0;a--);
}

//启动电机
//启动电机按照motorDirection设定的方向运转，同时开启timer0计数，
void startMotor()
{
	//设定方向
	RELAY_DIRECTION = motorDirection;
	
	delayRelay();
	
	//通电
	RELAY_POWER = 0;
	
	//打开timer0
	TR0 = 1;
}

//关闭电机，同时停止timer0
void stopMotor()
{
	//停马达电源
	RELAY_POWER = 1;
	
	//停止timer0
	TR0=0;
	
	delayRelay();
	
	//方向选择继电器断电
	RELAY_DIRECTION = 1;
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

//void delay10s(void)   //误差 0us
//{
//    unsigned char a,b,c,n;
//    for(c=191;c>0;c--)
//        for(b=209;b>0;b--)
//            for(a=249;a>0;a--);
//    for(n=4;n>0;n--);
//
//}
//
//
//void sendDataToHost( unsigned int tar, unsigned int cur, unsigned char c)
//{
//	unsigned char sendData[16];
//	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96频道，3字节地址，接收16字节
//	unsigned char tmp;
//
//		
//	sendData[0] = NODE_ID;//Node ID
//
//	sendData[1] = 11;
//	sendData[2] = 22;
//	sendData[3] = 33; 
//	sendData[4] = (tar >> 8); 
//	sendData[5] = (tar & 0x00ff); 
//	sendData[6] = (cur >> 8); 
//	sendData[7] = (cur & 0x00ff); 
//	sendData[8] = c ; 
//
//
//	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//向Pi发送, 96频道，3字节地址，16字节数据
//	
//	//24L01开始接收数据
//	startRecv(); 
//}


// 初始化Timer0, 4MHz, 每10ms触发一次中断
void initTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    //TR0 = 1; //start timer 0
}

//初始化继电器
void initRelays()
{
	RELAY_POWER=1;
	RELAY_DIRECTION=1;
}

//初始化卷帘位置
void initCurtain()
{
	//系统上电初始化时，我们要把卷帘全部卷上去。
	//为了让假设卷帘当前是在最下面，所以要全部卷上去，且要运行到顶校正余量位，以消除误差
	//假设卷帘当前是在最下面，所以要全部卷上去，且要运行到顶校正余量位，以消除误差
	
	currentPosTicks = POS_BOTTOM ; //假设当前在最底部，即全关(100%覆盖)
	targetPosTicks = CALIBRATION_LIMIT_TOP; //目标设为顶校正余量位
	motorDirection = DIRECTION_UP;
	curtainCoverPercent = 0; //全开，覆盖0%
	
	//启动电机
	startMotor();
}



void main()
{

	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//初始化中断0
	initINT0();
    
	//初始化继电器
	initRelays();
	
	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化timer0
	initTimer0();
	
	//初始化卷帘位置
	initCurtain();
	
	
	while(1)
	{
//		sendDataToHost(targetPosTicks, currentPosTicks , curtainCoverPercent);
//		delay10s();
	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char rCoverPercent;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): 发送者NodeID （目前忽略该参数）
	// *(receivedData+1): 功能号 （目前忽略该参数）
	// *(receivedData+2): 卷帘工作模式 （目前忽略该参数）
	// *(receivedData+3): 卷帘覆盖度百分比。例如要覆盖30%，则该值为30

	rCoverPercent = *(receivedData+3);
	

	if( rCoverPercent< 101 ) //rCoverPercent应该是[0,100]区间里面的一个数
	{
		if (curtainCoverPercent != rCoverPercent ) //如果收到的覆盖度与当前的覆盖度不同
		{
			curtainCoverPercent = rCoverPercent;
			
			if( rCoverPercent== 0 ) 
			{
				//如果用户选择“全开”，则利用这个机会进行校正
				//设置电机运行到顶校正余量位
				targetPosTicks = CALIBRATION_LIMIT_TOP ;
			}
			else if( rCoverPercent== 100 ) 
			{
				//如果用户选择“全关”，则利用这个机会进行校正
				//设置电机运行到底校正余量位			
				targetPosTicks = CALIBRATION_LIMIT_BOTTOM ;
			}
			else
			{
				//如果是介于(1%,100%)之间的覆盖度，则根据百分比计算电机运行目标位置
				targetPosTicks = POS_TOP + rCoverPercent /100.0 *(POS_BOTTOM - POS_TOP);
			}
			
			if( targetPosTicks != currentPosTicks)
			{
				//如果目标开度小于当前开度，则窗帘向上
				motorDirection = targetPosTicks<currentPosTicks?1:0;
				
				//启动马达
				startMotor();
			}

		}
	}
}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
	
	if(motorDirection) //相当于 if( motorDirection == DIRECTION_UP ), 窗帘向上运行
	{
		if( (--currentPosTicks <= targetPosTicks) || ( currentPosTicks < CALIBRATION_LIMIT_TOP  ) )
		{
			//停止马达
			stopMotor();
			
			//如果电机运行到校正余量，则调回顶位置
			if( currentPosTicks <= CALIBRATION_LIMIT_TOP )
			{
				currentPosTicks = POS_TOP;
			}
		}
	}
	else //相当于 else if( motorDirection == DIRECTION_DOWN )，窗帘向下运行
	{
		if( ( ++currentPosTicks >= targetPosTicks ) || ( currentPosTicks > CALIBRATION_LIMIT_BOTTOM  ) )
		{
			//停止马达
			stopMotor();
			
			//如果电机运行到校正余量，则调回底位置
			if(currentPosTicks >= CALIBRATION_LIMIT_BOTTOM)
			{
				currentPosTicks = POS_BOTTOM;
			}
		}
	}
    

}
