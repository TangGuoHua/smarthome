/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
DS18B20温度传感器
30A继电器模块

【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2014年02月05日  黄长浩  初始版本
2014年10月03日  黄长浩  增加小厨宝自动工作模式
                        增加EEPROM保存数据
						向Pi发送数据的延时由原10s延时计数改为timer0中断计数


【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

//STC11F04E 1T , 4MHz晶振

#include <reg52.h>
#include "nrf24L01Node.h"
#include "ds18B20.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 92

sfr AUXR   = 0x8E;

sbit RELAY_WATERHEATER = P3^7;  //小厨宝热水器控制继电器

volatile unsigned char waterHeaterMode = 0; //0：关，1：开，2：自动（受吊顶PIR控制）

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerCounter1s = 0;

//小厨宝烧水延时
volatile bit waterHeaterDelayOn = 0; //小厨宝烧水延时开始、停止
volatile int waterHeaterDelayCounter1s = 0; 
volatile unsigned char waterHeaterDelayMinute = 0; //小厨宝烧水延时分钟数

//开机延时 
//根据NodeID，进行约为(500*NodeID)毫秒的延时
//作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
void initDelay(void)
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=167;c>0;c--)
	        for(b=172;b>0;b--)
	            for(a=16;a>0;a--);
}


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


//初始化继电器
void initRelays()
{
	//全关
	RELAY_WATERHEATER=1;
}


//将小厨宝烧水延时分钟数存入片内eeprom
void saveWaterHeaterDelayMinute( unsigned char val )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, val );
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 92, 231, 92}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//向主机发送当前的温度和继电器状态
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	float temperature;
	int intTemperature;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update
	
	// DS18B20取得温度值
	temperature = readTemperature(1);
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
	sendData[2]= temperature<0 ? 1 : 0; //如果是零下则此位置1
	sendData[3]= intTemperature/10; //温度的整数部分
	sendData[4]= intTemperature%10; //温度的小数部分
	
	sendData[5]= waterHeaterMode;
	sendData[6]= waterHeaterDelayMinute;
	sendData[7] = (~RELAY_WATERHEATER); //Water heater relay status
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96频道，5字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv(); 
}



void main()
{
	//初始化中断0
	initINT0();
	
	//初始化继电器
	initRelays();
	
	//初始化DS18B20
	initDS18B20();
	
	//取得小厨宝烧水延时分钟数
	waterHeaterDelayMinute = eepromRead(0x0000);
	
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
		//向主机发送数据
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();	
		}
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char tmp;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	if( *receivedData == 1 )//来自Pi(#1)
	{
		//小厨宝工作模式
		tmp = *(receivedData+1);
		
		//小厨宝控制
		if( tmp == 0 )//常关
		{
			RELAY_WATERHEATER = 1;
			
			//如果之前小厨宝在自动模式并且正在延时烧水过程中，
			//则停止烧水延时计数器，避免混乱
			waterHeaterDelayOn = 0; 
		}
		else if( tmp == 1 )//常开
		{
			RELAY_WATERHEATER = 0;
			
			//如果之前小厨宝在自动模式并且正在延时烧水过程中，
			//则停止烧水延时计数器，避免混乱
			waterHeaterDelayOn = 0; 
		}
		else if( tmp == 2 )//自动
		{
			//无论之前小厨宝是否在烧水，或工作于何种模式，
			//均停止烧水，停止延时计数
			//进入自动模式2后，系统将忽略当前是否有人的状态
			//只有等待下一次卫生间由无人变有人时，才开始自动烧水
			RELAY_WATERHEATER = 1;
			waterHeaterDelayOn = 0; 
		}
		waterHeaterMode = tmp;
		
		
		//烧水延时分钟数	
		tmp = *(receivedData+2);
		if( tmp != waterHeaterDelayMinute )//分钟数变化了
		{
			waterHeaterDelayMinute=tmp;
			saveWaterHeaterDelayMinute( tmp );
		}
	}
	else if( *receivedData == 91 ) //来自吊顶控制器(#92)
	{
		//小厨宝工作在自动模式吗？
		//如果不在自动模式，则忽略来自吊顶控制器PIR的数据
		if( waterHeaterMode == 2 ) 
		{
			tmp = *(receivedData+1);
			if( tmp == 1)
			{
				//人来了，烧水
								
				waterHeaterDelayOn = 0; //烧水延时计时停止
				waterHeaterDelayCounter1s = 0;
				
				RELAY_WATERHEATER = 0; //开始烧水
			}
			else if( tmp == 0 )
			{
				//人走了，继续烧水(waterHeaterDelayMinute)分钟，然后再关小厨宝
				waterHeaterDelayCounter1s = 0;
				waterHeaterDelayOn = 1; //烧水延时开始计时
			}
		}
	}
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
			sendDataNow = 1;
		}
		
		//小厨宝延时
		if( waterHeaterDelayOn )
		{
			if( ++waterHeaterDelayCounter1s >= waterHeaterDelayMinute*60 )
			{
				//延时已到
				waterHeaterDelayOn = 0; //停止延时计时
				waterHeaterDelayCounter1s = 0; //计数器清零
				RELAY_WATERHEATER = 1; //关小厨宝
			}
		}
	}
}