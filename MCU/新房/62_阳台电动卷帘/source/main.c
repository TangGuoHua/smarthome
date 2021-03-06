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
2014年10月19日  黄长浩  升级nrf驱动
                        增加BH1750/DS18B20

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "bh1750fvi.h"
#include "ds18B20.h"


sfr AUXR   = 0x8E;

// Node ID
#define NODE_ID 62

/*
行程控制说明:

系统通过控制电机运行时间（即ticks计数）来控制窗帘的位置。
系统设定timer0每10ms中断一次，产生一个tick，通过对ticks计数就可以知道电机运行时间。
通过实验，卷帘从最上到最下（下行）运行约41.5秒，上行全程运行约40.5秒。
在计算的时候，上下行程都按全程41.5秒计算。每10ms一个tick，则全程4150个ticks。

考虑到使用unsigned int来计数ticks，为避免出现0及负数，我们以500做为顶位置计数，4650最为最底位置计数

最大行程时间为 4650-500=4150 ticks， 即4150*10=41500ms=41.5s

当用户选择全开（0%覆盖）或者全关（100%覆盖）时，系统将多运行300*10ms（即3秒），以消除累计误差
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

volatile unsigned char curtainMode = 0; //卷帘工作模式
volatile unsigned char curtainCoverPercent; //卷帘覆盖度百分比

//马达运动方向
volatile bit motorDirection = DIRECTION_UP; 

//卷帘当前位置 10ms ticks
volatile unsigned int currentPosTicks = 0;

//目标位置 10ms ticks
volatile unsigned int targetPosTicks = 0;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char sendDataTimer10sCount = 0;
volatile unsigned char functionNum = 0;


void delayMainInterval(void)
{
	//延时10秒 @STC12C5616AD, 4MHz晶振
    unsigned char a,b,c,n;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
    for(n=4;n>0;n--);
}


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


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 62, 231, 62}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr); //接收92频道
}



// 初始化Timer0, 4MHz, 每10ms触发一次中断
void initTimer0(void)
{
	AUXR = AUXR|0x80;  //设置T0为1T模式
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
	//由于刚上电,系统不知道当前卷帘的位置,而卷帘可以在任何位置。
	//为了让卷帘全部卷上去，且要运行到顶校正余量位（以消除误差），我们要假设卷帘位置在最下面
	//系统按最长时间给电，以便让卷帘走完整个行程
	
	currentPosTicks = POS_BOTTOM ; //假设当前在最底部，即全关(100%覆盖)
	targetPosTicks = CALIBRATION_LIMIT_TOP; //目标设为顶校正余量位
	motorDirection = DIRECTION_UP;
	curtainCoverPercent = 0; //全开，覆盖0%
	
	//启动电机
	startMotor();
}


//向主机发送数据
void sendDataToHost( float temperature, unsigned int brightness )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3字节地址
	unsigned char ret;
	int intTemperature;

	sendData[0] = NODE_ID;//Node ID
	sendData[1] = functionNum;// Function Number, 1-regular status update, 51-respond to 50

	sendData[2] = curtainMode; // working mode
	sendData[3] = curtainCoverPercent;
	
	sendData[4] = ~RELAY_POWER;
	sendData[5] = ~RELAY_DIRECTION;
	
	//亮度值
	sendData[6] = brightness/256;
	sendData[7] = brightness%265;
	
	//温度值
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
	sendData[8] = temperature<0 ? 1 : 0; //如果是零下则此位置1
	sendData[9] = intTemperature/10; //温度的整数部分
	sendData[10] = intTemperature%10; //温度的小数部分

	//发送数据
	TR0 = 0;//Pause timer0
	ret = nrfSendData( 96, 5, toAddr, 16, sendData);
	
	//24L01开始接收数据
	startRecv();
	TR0 = 1;//Resume timer0
	
}


//主程序
void main()
{
	float temperature;
	unsigned int brightness;
	
	//初始化中断0
	initINT0();
    
	//初始化继电器
	initRelays();
	
	//初始化24L01
	nrf24L01Init();
	
	// 初始化 BH1750FVI
	bh1750Init();
	
	//初始化DS18B20
	initDS18B20();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化timer0
	initTimer0();
	
	//初始化卷帘位置
	initCurtain();
	
	
	while(1)
	{
		// DS18B20取得温度值
		temperature = readTemperature(0);
		
		//读取亮度值
		brightness = bh1750GetBrightness();
		
		//定时状态报告
		if( ++sendDataTimer10sCount >= 600 ) //每10*N秒发送一次数据
		{
			sendDataTimer10sCount = 0;
			functionNum = 1; //regular status update
			sendDataNow = 1;
			
		}
		
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost( temperature, brightness );
		}
		
		delayMainInterval();

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
	// *(receivedData+1): 功能号 
	// *(receivedData+2): 卷帘工作模式 （目前忽略该参数）
	// *(receivedData+3): 卷帘覆盖度百分比。例如要覆盖30%，则该值为30
	
	
	switch( *(receivedData+1) )
	{
		case 20: //执行命令，无需返回

			curtainMode = *(receivedData+2);
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
						targetPosTicks = POS_TOP + rCoverPercent/100.0 *(POS_BOTTOM - POS_TOP);
					}
					
					if( targetPosTicks != currentPosTicks)
					{
						//如果目标开度小于当前开度，则窗帘向上
						motorDirection = targetPosTicks<currentPosTicks ? DIRECTION_UP : DIRECTION_DOWN;
						
						//启动马达
						startMotor();
					}
				}
			}
			break;
		

		
		case 50: //请求立即报告状态
			functionNum = 51;//回报50
			sendDataTimer10sCount = 0; //重新计时
			sendDataNow = 1;
			break;
	
	}//end of switch
}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
	
	if(motorDirection) //相当于 if( motorDirection == DIRECTION_UP ), 窗帘向上运行
	{
		//如果已经到达了目标位置或者超过了顶校正余量限位，则停止马达
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
		//如果已经到达了目标位置或者超过了底校正余量限位，则停止马达
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
