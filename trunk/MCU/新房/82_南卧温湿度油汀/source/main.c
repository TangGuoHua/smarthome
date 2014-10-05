/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
奥松AM2321温湿度传感器
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
#include "am2321.h"

// Node ID
#define NODE_ID 82

sbit RELAY_HEATER = P3^7;
unsigned char gRelayMode = 0;  //0：常关，1：常开，2：温控
unsigned char gSetTemperatureX10; //设定温度


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
	unsigned char myAddr[3]= {97, 83, 82}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr);
}


//向主机发送当前的温度和继电器状态
void sendDataToHost( unsigned char readAm2321Result )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 3字节地址
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	
	sendData[1] = readAm2321Result;
	sendData[2] = getAM2321Data(0);
	sendData[3] = getAM2321Data(1);
	sendData[4] = getAM2321Data(2);
	sendData[5] = getAM2321Data(3);
	sendData[6] = getAM2321Data(4);
	
	sendData[7] = ~RELAY_HEATER;
	sendData[8] = gRelayMode;
	sendData[9] = gSetTemperatureX10;
	
	
	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//Pi, 96频道，3字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;
	unsigned char readAm2321Result = 0;
	int temperatureX10 = 0;
	
	RELAY_HEATER=1;
	
	
	//初始化中断0
	initINT0();
	
	//init 2321
	initAM2321();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	initDelay();
			
	while(1)
	{
		
		//触发AM3231测量，并读取上一次测量的温湿度值
		readAm2321Result = readAM2321();


		if( readAm2321Result == 0 ) //读取am2321成功
		{
			// 工作在自动温控模式
			if(gRelayMode==2)
			{
				//获得当前温度 10倍数
				temperatureX10 = getAM2321Data(2)*10 + getAM2321Data(3);
				if( getAM2321Data(4) == 1 )
				{
					//负温度
					temperatureX10 *= -1;
				}
				
				if( temperatureX10>=(gSetTemperatureX10+1) ) //超过设定值0.1度
				{
					RELAY_HEATER = 1; //关继电器，停止加热
					//sendDataToHost( temperatureX10, 0 );
				}
				else if( temperatureX10<=(gSetTemperatureX10-1) ) //低于设定值0.1度
				{
					RELAY_HEATER = 0; //开继电器，开始加热
					//sendDataToHost( temperatureX10, 0 );
				}
			}	
			
			//向主机发送数据
			if( ++sendDataTimerCount >= 60 )  //如果设置为60，则每60*10＝600秒发送一次数据
			{
				sendDataToHost( readAm2321Result );	
				sendDataTimerCount=0;
			}
		}

		delay10s(); //延时10秒
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	//字节1，控制油汀继电器的工作模式 （0：常关，1：常开，2：温控）
	gRelayMode = *(receivedData++);
	if( gRelayMode == 0 ) RELAY_HEATER=1;
	else if( gRelayMode == 1 ) RELAY_HEATER=0;
	
	//字节2，设定温度 10倍数
	//只能设定0－255，即0度至25.5度，之间的值
	//考虑到本节点只用于冬天控制油汀，这个设定温度区间应该是合理的
	gSetTemperatureX10 = *(receivedData++);
}
