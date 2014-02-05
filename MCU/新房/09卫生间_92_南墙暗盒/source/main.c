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

// Node ID
#define NODE_ID 92


sbit RELAY_WATERHEATER = P3^7;  //小厨宝热水器控制继电器


float gTemperature; //DS18B20测得的温度

//开机延时 
//根据NodeID，进行约为(500*NodeID)毫秒的延时
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


void delay10s(void) 
{

	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 92}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr);
}


//向主机发送当前的温度和继电器状态
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 3字节地址
	unsigned char tmp;
	
	int intTemperature;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = (~RELAY_WATERHEATER); //Water heater relay status
	
	// DS18B20取得温度值
	//temperature = readTemperature(1);

	//四舍五入到十分位
	intTemperature = gTemperature<0? (gTemperature*(-10)+0.5) : gTemperature*10+0.5; 
	
	sendData[2]= gTemperature<0 ? 1 : 0; //如果是零下则此位置1
	sendData[3]= intTemperature/10; //温度的整数部分
	sendData[4]= intTemperature%10; //温度的小数部分
	
	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96频道，3字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;

	//初始化中断0
	initINT0();
	
	//初始化DS18B20
	initDS18B20();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	initDelay();
			
	while(1)
	{
		// DS18B20取得温度值
		gTemperature = readTemperature(0);	
		
		//向主机发送数据
		if( ++sendDataTimerCount >= 60 )  //如果设置为60，则每60*10＝600秒发送一次数据
		{
			sendDataToHost();	
			sendDataTimerCount=0;
		}
		
		delay10s(); //延时约10秒
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	//字节1，控制1号继电器工作模式（0：常关，1：常开，2：延时关）
	RELAY_WATERHEATER = (*(receivedData++)==0)?1:0;

}
