/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
人体红外PIR探头
继电器模块
DS18B20温度探头

【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年09月26日  黄长浩  初始版本
2014年01月27日  黄长浩  增加initDelay()
2014年02月09日  黄长浩  1号、2号灯分别设置开灯阈值
2014年10月02日  黄长浩  更新nrf24l01+驱动
                        传回Pi的数据增加灯工作模式值


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
#include "ds18B20.h"

// Node ID
#define NODE_ID 22

sfr AUXR   = 0x8E;

//sbit LDR = P1^7;  //光敏电阻 （10K上拉）
sbit PIR = P1^3;  //热释电红外传感器

sbit RELAY_LIGHT1 = P1^5; //灯控继电器1 （柜灯）
sbit RELAY_LIGHT2 = P1^4; //灯控继电器2 （顶灯）


volatile unsigned char light1Mode = 2; //0：常关，1：常开，2：自动
volatile unsigned char light2Mode = 2; //0：常关，1：常开，2：自动
volatile unsigned char light1OnThreshold = 100; // 开1号灯的阈值
volatile unsigned char light2OnThreshold = 80; // 开2号灯的阈值

volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerSendData = 0;

// Flag for sending data to Pi
bit sendDataNow = 0;


//开机延时 
//根据NodeID，进行约为500*NodeID毫秒的延时
//作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
void initDelay(void)
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=167;c>0;c--)
	        for(b=171;b>0;b--)
	            for(a=16;a>0;a--);
}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}




//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 22, 231, 231}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr); //接收92频道，5字节地址
}


//返回当前亮度值
unsigned char getBrightness()
{
	return (255-getADCResult(7));
}



//将开灯阈值存入片内eeprom
void saveLightOnThreshold( unsigned char threshold1, unsigned char threshold2 )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, threshold1 );
	eepromWrite( 0x0002, threshold2 );
}

////从片内eeprom取得开灯阈值
//unsigned char getLightOnThreshold()
//{
//	return eepromRead(0x0000);
//}


void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 96频道，5字节地址，接收16字节
	unsigned char tmp;
	float temperature;
	int intTemperature;
		
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update
	sendData[2] = PIR;
	sendData[3] = getBrightness(); //亮度
	sendData[4] = light1OnThreshold;
	sendData[5] = light2OnThreshold;
	sendData[6] = light1Mode;
	sendData[7] = light2Mode;
	sendData[8] = ~RELAY_LIGHT1;
	sendData[9] = ~RELAY_LIGHT2;

	// DS18B20取得温度值
	temperature = readTemperature(1);

	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
	
	sendData[10]= temperature<0 ? 1 : 0; //如果是零下则此位置1
	sendData[11]= intTemperature/10; //温度的整数部分
	sendData[12]= intTemperature%10; //温度的小数部分

	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//向Pi发送, 96频道，3字节地址，16字节数据
	
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
    TR0 = 1;
}

void initRelays()
{
	RELAY_LIGHT1=1;
	RELAY_LIGHT2=1;
}

void main()
{

	bit lastPIR=0;
	bit thisPIR=0;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//初始化中断0
	initINT0();
    
	//初始化继电器
	initRelays();
	
	//初始化DS18B20
	initDS18B20();
	
	//初始化ADC
	initADC();
	
	//取得开灯阈值
	light1OnThreshold = eepromRead(0x0000);
	light2OnThreshold = eepromRead(0x0002);

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

		//Send data to Pi
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();
		}
		
		// 灯控1
		if( light1Mode == 0 ) //常关
		{
			RELAY_LIGHT1=1;
		}
		else if( light1Mode ==1 ) //常开
		{
			RELAY_LIGHT1=0;
		}
		else if( light1Mode ==2 ) //自动
		{
			if(RELAY_LIGHT1) //当前灯是灭的
			{
				if( thisPIR && getBrightness()<=light1OnThreshold ) //有人，且环境亮度在阈值以下
				{
					RELAY_LIGHT1 = 0; //开灯
				}
			}
			else //当前灯是亮的
			{
				RELAY_LIGHT1 = ~thisPIR; //继续亮，或关灯
			}
		}
		
		// 灯控2
		if( light2Mode == 0 ) //常关
		{
			RELAY_LIGHT2=1;
		}
		else if( light2Mode ==1 ) //常开
		{
			RELAY_LIGHT2=0;
		}
		else if( light2Mode ==2 ) //自动
		{
			if(RELAY_LIGHT2) //当前灯是灭的
			{
				if( thisPIR && getBrightness()<=light2OnThreshold ) //有人，且环境亮度在阈值以下
				{
					RELAY_LIGHT2 = 0; //开灯
				}
			}
			else //当前灯是亮的
			{
				RELAY_LIGHT2 = ~thisPIR; //继续亮，或关灯
			}
		}		
		
		//记录当前值
		lastPIR = thisPIR;

	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char tmp1, tmp2;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): 开1号灯的亮度阈值
	// *(receivedData+1): 开2号灯的亮度阈值
	// *(receivedData+2): 灯控继电器1的工作模式
	// *(receivedData+3): 灯控继电器2的工作模式

	
	//开灯阈值
	tmp1 = *(receivedData+0);
	tmp2 = *(receivedData+1);
	if( tmp1 != light1OnThreshold || tmp2 != light2OnThreshold )
	{
		//threshold changed, let's save it.
		light1OnThreshold = tmp1;
		light2OnThreshold = tmp2;
		saveLightOnThreshold( tmp1, tmp2 );
	}
	
	light1Mode = *(receivedData+2);
	light2Mode = *(receivedData+3);
	

}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100个10ms，即1秒
	{
		timerCounter10ms=0;
		
		if( ++timerSendData == 600 ) //每600秒向Pi发送一次数据
		{
			timerSendData = 0;
			sendDataNow = 1;
		}
		

	}
}
