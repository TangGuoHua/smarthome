/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
人体红外PIR探头
继电器模块
光敏电阻

【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2013年10月01日  黄长浩  初始版本
2014年01月27日  黄长浩  增加initDelay()
2014年10月03日  黄长浩  增加向92号节点发送数据的逻辑
                        升级NRF24L01+驱动
2014年10月06日  黄长浩  修改initDelay()延时为(NodeID*2)秒
					
【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "adc.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 91

sfr AUXR   = 0x8E;

sbit LIGHT_R = P1^4;  //光敏电阻 （10K上拉）
sbit PIR = P3^5;      //热释电红外传感器
sbit RELAY_HEATER = P3^7; //热水器继电器（30A）
sbit RELAY_LIGHT1 = P1^7; //灯控继电器1
sbit RELAY_LIGHT2 = P1^6; //灯控继电器2


volatile unsigned char gLight1Mode = 2; //0：常关，1：常开，2：自动
volatile unsigned char gLightOnThreshold = 90; // 开灯的阈值

volatile unsigned char gHeaterMode = 0; //0：常关，1：常开

volatile unsigned char gTimerCounter10ms = 0;
volatile unsigned int gTimerSendData = 0;

// Flag for sending data to Pi
volatile bit gSendDataFunctNum = 0;



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
	//10秒
    unsigned n;
    for(n=0;n<2;n++)
	{
		delay5s();
	}

}



void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 91, 231, 91}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//返回当前亮度值
unsigned char getBrightness()
{
	return (255-getADCResult(4));
}


//将开灯阈值存入片内eeprom
void saveLightOnThreshold( unsigned char x )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, x );
}


//从片内eeprom取得开灯阈值
unsigned char getLightOnThreshold()
{
	return eepromRead(0x0000);
}


//发送数据给Pi
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1] = gSendDataFunctNum; //Function Number

	sendData[2] = PIR;
	sendData[3] = getBrightness(); //亮度
	sendData[4] = gLightOnThreshold; 
		
	sendData[5] = ~RELAY_LIGHT1;
	sendData[6] = ~RELAY_LIGHT2;
	sendData[7] = ~RELAY_HEATER;
	
	//Stop timer0
	TR0=0;
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96频道，5字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv();
	
	//Start timer0
	TR0=1;
}


//发送数据给92号节点
//供控制小厨宝用
void sendDataToNode92( unsigned char val )
{
	unsigned char sendData[6];
	unsigned char toAddr[5]= {97,83,92,231,92}; //Node 92地址
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1]= 2; //特定事件报告
	sendData[2]= 0; //跳过（accident）
	sendData[3]= val;

	//Stop timer0
	TR0=0;
	
	tmp = nrfSendData( 92, 5, toAddr, 6, sendData);//Pi, 92频道，5字节地址，6字节数据
	
	//24L01开始接收数据
	startRecv();
	
	//Start timer0
	TR0=1;
	
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


void initRelays()
{
	RELAY_LIGHT1=1;
	RELAY_LIGHT2=1;
	RELAY_HEATER=1;
}


void main()
{

	bit lastPIR=0;
	bit thisPIR=0;
	
	//初始化中断0
	initINT0();
    
	//初始化继电器
	initRelays();
	
	//初始化ADC
	initADC();
	
	//取得开灯阈值
	gLightOnThreshold = getLightOnThreshold();

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
		//取得传感器的当前值
		thisPIR = PIR;

		// 灯控1
		if( gLight1Mode == 0 ) //常关
		{
			RELAY_LIGHT1=1;
		}
		else if( gLight1Mode ==1 ) //常开
		{
			RELAY_LIGHT1=0;
		}
		else if( gLight1Mode ==2 ) //自动
		{
			if(RELAY_LIGHT1) //当前灯是灭的
			{
				if( thisPIR && getBrightness()<=gLightOnThreshold ) //有人，且环境亮度在阈值以下
				{
					RELAY_LIGHT1 = 0; //开灯
				}
				
			}
			else //当前灯是亮的
			{
				RELAY_LIGHT1 = ~thisPIR; //继续亮，或关灯
			}
		}
		
		//当有人无人状态发生转变时，将数据传给92号节点
		//用于控制小厨宝（NodeID:92）		
		if( thisPIR != lastPIR ) 
		{
			sendDataToNode92( thisPIR );
		}
		
		//记录当前值
		lastPIR = thisPIR;
		
		
		//Send data to Pi
		if( gSendDataFunctNum )
		{
			sendDataToHost(  );
			gSendDataFunctNum = 0;
		}

	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char fromNodeID, funcNum;
	unsigned char tmp;
	
	//Stop timer0
	TR0=0;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	//Start timer0
	TR0=1;
	
	// *(receivedData+0): From Node ID, 固定为1
	// *(receivedData+1): Function Number
	// *(receivedData+2): 开灯的亮度阈值
	// *(receivedData+3): 灯控继电器1的工作模式
	// *(receivedData+4): 灯控继电器2的工作模式
	// *(receivedData+5): 热水器工作模式
	// *(receivedData+6): 热水器烧水分钟数
	

	
	fromNodeID = *receivedData;
	funcNum = *(receivedData+1);
	
	if( fromNodeID == 1 )//来自Pi(#1)
	{
		if( funcNum == 20 || funcNum == 22)
		{
	
			//开灯阈值
			tmp = *(receivedData+2);
			if( tmp!= gLightOnThreshold )
			{
				//threshold changed, let's save it.
				gLightOnThreshold = tmp;
				saveLightOnThreshold( gLightOnThreshold );
			}
			
			//灯控1的模式
			gLight1Mode = *(receivedData+3);
			
			//灯控2暂时没用
			
			//热水器工作模式
			tmp = *(receivedData+5);
			if( tmp != gHeaterMode ) //模式变化了
			{
				if( tmp==0 )
				{
					//关
					RELAY_HEATER = 1;
				}
				else if( tmp==1 )
				{
					//开
					RELAY_HEATER = 0;
				}
				else if( tmp==2 )
				{
					//延时
				}
				gHeaterMode = tmp;
			}
		}
		
		if( funcNum == 22 )
		{
			gSendDataFunctNum = 23;
		}
		else if( funcNum == 50 )
		{
			gSendDataFunctNum = 51;
		}		
	}
}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++gTimerCounter10ms == 100 ) //100个10ms，即1秒
	{
		gTimerCounter10ms=0;
		
		if( ++gTimerSendData == 600 ) //每600秒向Pi发送一次数据
		{
			gTimerSendData = 0;
			gSendDataFunctNum = 1;
		}
	}
}

