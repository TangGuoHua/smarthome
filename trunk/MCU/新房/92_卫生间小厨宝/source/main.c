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
2014年12月28日  黄长浩  增加空气加热器（快热炉）控制部分

--------------
!! TODO: 
在数传模块收发数据时要停止timer，避免因中断造成时序混乱 （参考91号节点的程序）


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
sbit RELAY_AIRHEATER1 = P1^4;
sbit RELAY_AIRHEATER2 = P1^5;

volatile unsigned char gWaterHeaterMode = 0; //0：关，1：开，2：自动（受吊顶PIR控制）
volatile unsigned char gAirHeaterMode = 0; //0：关，1：开，2：温控
volatile unsigned char gSetTemperatureX10 = 0; //设定的温度 (x10)。当air heater工作模式=2时，要求达到的温度值
volatile int gCurrentTemperatureX10=0; //当前温度 (x10)

// Flag for sending data to Pi
volatile unsigned char gSendDataFunctNum = 0;
volatile unsigned char gTimerCounter10ms = 0;
volatile unsigned int gTimerCounter1s = 0;

//小厨宝烧水延时
volatile bit gWaterHeaterDelayOn = 0; //小厨宝烧水延时开始、停止
volatile int gWaterHeaterDelayCounter1s = 0; 
volatile unsigned char gWaterHeaterDelayMinute = 0; //小厨宝烧水延时分钟数

volatile bit gPeopleIn = 0; //有人在吗？


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
	//50秒
    unsigned n;
    for(n=0;n<10;n++)
	{
		delay5s();
	}

}


//延时2秒
void delay2s(void)
{
    unsigned char a,b,c;
    for(c=252;c>0;c--)
        for(b=230;b>0;b--)
            for(a=33;a>0;a--);
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
	RELAY_AIRHEATER1 = 1;
	RELAY_AIRHEATER2 = 1;	
}


//打开关闭加热器
//switchOn=0:关
//switchOn=1:开
void switchAirHeater( bit switchOn )
{
	if( switchOn )
	{
		RELAY_AIRHEATER1 = 0;
		RELAY_AIRHEATER2 = 0;
	}
	else
	{
		RELAY_AIRHEATER1 = 1;
		RELAY_AIRHEATER2 = 1;	
	}
}


//保存设置参数到片内eeprom
void saveSettings()
{
	eepromEraseSector( 0x0000 );
	
	//保存小厨宝烧水延时分钟数
	eepromWrite( 0x0000, gWaterHeaterDelayMinute );
	
	//保存设定温度
	eepromWrite( 0x0002, gSetTemperatureX10 );	
}


int getTemperatureX10()
{
	float temperature;
	int intTemperature;
	
	// DS18B20取得温度值
	temperature = readTemperature(0);
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
	return (intTemperature*(temperature<0 ? -1 : 1));
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 92, 231, 92}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//向主机发送当前的温度和继电器状态
void sendDataToHost( unsigned char funcNum )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;

	sendData[0]= NODE_ID;//Node ID
	sendData[1]= funcNum; //function number
	
	
	sendData[2]= gCurrentTemperatureX10<0 ? 1 : 0; //如果是零下则此位置1
	sendData[3]= ( gCurrentTemperatureX10*(gCurrentTemperatureX10<0 ? -1 : 1) )/10; //温度的整数部分
	sendData[4]= ( gCurrentTemperatureX10*(gCurrentTemperatureX10<0 ? -1 : 1) )%10; //温度的小数部分
	
	sendData[5]= gWaterHeaterMode;
	sendData[6]= gWaterHeaterDelayMinute;
	sendData[7]= (~RELAY_WATERHEATER); //Water heater relay status
	
	sendData[8]= gAirHeaterMode;
	sendData[9]= gSetTemperatureX10;
	sendData[10]= (~RELAY_AIRHEATER1);
	sendData[11]= (~RELAY_AIRHEATER2);
	
	
	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96频道，5字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv(); 
}



void main()
{

	//设置T0为1T模式
	AUXR = AUXR|0x80;  
	
	//初始化中断0
	initINT0();
	
	//初始化继电器
	initRelays();
	
	//初始化DS18B20
	readTemperature(0);
	
	//取得小厨宝烧水延时分钟数
	gWaterHeaterDelayMinute = eepromRead(0x0000);
	//取得设定的温度
	gSetTemperatureX10 = eepromRead(0x0002);
	
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
		//每圈延时2秒，以便于ds18b20测温
		delay2s();
		
		//测温
		gCurrentTemperatureX10 = getTemperatureX10();
		
		//加热器工作在温控模式吗？
		if( gAirHeaterMode == 2 )
		{
			if( gCurrentTemperatureX10 >= gSetTemperatureX10 + 1 )
			{
				switchAirHeater(0);
			}
			else if( gCurrentTemperatureX10 <= gSetTemperatureX10 - 1 )
			{
				switchAirHeater(1);
			}
		}
		
		//向主机发送数据
		if( gSendDataFunctNum !=0  )
		{
			sendDataToHost( gSendDataFunctNum );
			gSendDataFunctNum = 0;
			gTimerCounter1s = 0; //重新开始计时
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
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	fromNodeID = *receivedData;
	funcNum = *(receivedData+1);
	
	if( fromNodeID == 1 )//来自Pi(#1)
	{
		if( funcNum == 20 || funcNum == 22)
		{
			//小厨宝工作模式
			tmp = *(receivedData+2);
			
			if( gWaterHeaterMode != tmp ) //小厨宝工作模式变化了
			{
			
				//小厨宝控制
				if( tmp == 0 )//常关
				{
					RELAY_WATERHEATER = 1;
					
					//如果之前小厨宝在自动模式并且正在延时烧水过程中，
					//则停止烧水延时计数器，避免混乱
					gWaterHeaterDelayOn = 0; 
				}
				else if( tmp == 1 )//常开
				{
					RELAY_WATERHEATER = 0;
					
					//如果之前小厨宝在自动模式并且正在延时烧水过程中，
					//则停止烧水延时计数器，避免混乱
					gWaterHeaterDelayOn = 0; 
				}
				else if( tmp == 2 )//自动
				{
					if( gPeopleIn )
					{
						//如果有人，则开始烧水
						gWaterHeaterDelayOn = 0; //烧水延时计时停止
						gWaterHeaterDelayCounter1s = 0;
						
						RELAY_WATERHEATER = 0; //开始烧水			
					}
					else
					{
						//如果没人，则停止烧水
						RELAY_WATERHEATER = 1;
						gWaterHeaterDelayOn = 0;
					}
				}
				gWaterHeaterMode = tmp;
			}
			
			
			//烧水延时分钟数	
			tmp = *(receivedData+3);
			if( tmp != gWaterHeaterDelayMinute )//分钟数变化了
			{
				gWaterHeaterDelayMinute=tmp;
				saveSettings();
			}
			
			
			//温度设定
			//允许值范围 10-250，即1-25度
			//
			tmp = *(receivedData+5);
			if( tmp != gSetTemperatureX10 )//设定的温度变化了
			{
				gSetTemperatureX10=tmp;
				saveSettings();
			}
			
			
			//加热器工作模式	
			tmp = *(receivedData+4);			
			if( tmp != gAirHeaterMode )
			{
				if( tmp == 0 )
				{
					//关闭加热器
					switchAirHeater(0);
				}
				else if( tmp == 1 )
				{
					//打开加热器
					switchAirHeater(1);
				}
				else if( tmp == 2 )
				{
					//设置为温控模式
					if( gCurrentTemperatureX10 >= gSetTemperatureX10 + 1 )
					{
						switchAirHeater(0);
					}
					else if( gCurrentTemperatureX10 <= gSetTemperatureX10 - 1 )
					{
						switchAirHeater(1);
					}
				}
				
				gAirHeaterMode = tmp;

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
	else if( fromNodeID == 91 ) //来自吊顶控制器(#91)
	{
		if( funcNum == 2) //事件报告
		{
			//有人在吗？
			gPeopleIn = (*(receivedData+3)==0)?0:1;
			
			//小厨宝工作在自动模式吗？
			//如果不在自动模式，则忽略来自吊顶控制器PIR的数据
			if( gWaterHeaterMode == 2 ) 
			{
				if( gPeopleIn )
				{
					//人来了，烧水
									
					gWaterHeaterDelayOn = 0; //烧水延时计时停止
					gWaterHeaterDelayCounter1s = 0;
					
					RELAY_WATERHEATER = 0; //开始烧水
				}
				else
				{
					//人走了，继续烧水(waterHeaterDelayMinute)分钟，然后再关小厨宝
					gWaterHeaterDelayCounter1s = 0;
					gWaterHeaterDelayOn = 1; //烧水延时开始计时
				}
			}
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
		
		//向Pi发送数据
		if( ++gTimerCounter1s == 600 ) //每600秒一次
		{
			gTimerCounter1s = 0;
			gSendDataFunctNum = 1; //发送数据
		}
		
		//小厨宝延时
		if( gWaterHeaterDelayOn )
		{
			if( ++gWaterHeaterDelayCounter1s >= gWaterHeaterDelayMinute*60 )
			{
				//延时已到
				gWaterHeaterDelayOn = 0; //停止延时计时
				gWaterHeaterDelayCounter1s = 0; //计数器清零
				RELAY_WATERHEATER = 1; //关小厨宝
			}
		}
	}
}