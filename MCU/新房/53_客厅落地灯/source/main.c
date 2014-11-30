/***************************************************************************
【硬件信息】
单片机型号: STC11F04E
工作频率: 4MHz 外部晶振
其它：
继电器模块10A*2路
AM2321

【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2014年03月29日  黄长浩  初始版本
2014年10月12日  黄长浩  升级nrf驱动
                        增加AM2321
						调整初始延时
2014年10月12日  黄长浩  增加取暖器温控

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
#define NODE_ID 53

sfr AUXR   = 0x8E;

sbit RELAY_LAMP = P1^5;
sbit RELAY_NOT_USED = P1^4; //没有用到的继电器

sbit RELAY_HEATER1 = P1^7;
sbit RELAY_HEATER2 = P1^6; 


unsigned char gSendDataNow = 0; //马上发送数据

unsigned char gHeaterMode = 0;  //0：常关，1：常开，2：温控
unsigned char gSetTemperatureX10 = 0; //设定温度

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
	//30秒
    unsigned n;
    for(n=0;n<6;n++)
	{
		delay5s();
	}

}


//延时10秒 
void delay10s(void) 
{
	//4MHz Crystal, 1T STC11F04E
    unsigned char a,b,c;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//开启、关闭取暖器
// onOff = 1，开启
// onOff = 0，关闭
void switchHeater( unsigned char onOff )
{
	if( onOff==1 ) //switch on
	{
		RELAY_HEATER1=0;
		RELAY_HEATER2=0;
	}
	else if( onOff == 0 ) // switch off
	{
		RELAY_HEATER1=1;
		RELAY_HEATER2=1;	
	}
}


void initRelays()
{
	//初始化继电器
	RELAY_LAMP = 1;
	RELAY_NOT_USED = 1;
	switchHeater(0);
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 53, 231, 53}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//向主机发送当前的温度和继电器状态
void sendDataToHost( unsigned char functionNum, unsigned char readAm2321Result )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3字节地址
	unsigned char tmp;
	
	sendData[0] = NODE_ID;//Node ID
	sendData[1] = functionNum;// Function Number, 1-regular status update, 51-respond to 50
	sendData[2] = readAm2321Result;
	sendData[3] = getAM2321Data(0);
	sendData[4] = getAM2321Data(1);
	sendData[5] = getAM2321Data(2);
	sendData[6] = getAM2321Data(3);
	sendData[7] = getAM2321Data(4);
	sendData[8] = ~RELAY_LAMP;
	sendData[9] = gHeaterMode;
	sendData[10] = gSetTemperatureX10;
	sendData[11] = ~RELAY_HEATER1;
	sendData[12] = ~RELAY_HEATER2;
	
	tmp = nrfSendData(96, 5, toAddr, 16, sendData);
	
	//24L01开始接收数据
	startRecv();
}


void main()
{
	unsigned char readAm2321Result = 0;
	int temperatureX10 = 0;
	unsigned char timerCounter10s = 0;
	
	//初始化继电器
	initRelays();
	
	//init AM2321
	initAM2321();	
	
	//初始化中断0
	initINT0();
	
	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	initDelay();


	while(1)
	{
		
		//触发AM3231测量，并读取上一次测量的温湿度值
		readAm2321Result = readAM2321(1);


		if( readAm2321Result == 0 ) //读取am2321成功
		{
			// 工作在自动温控模式
			if(gHeaterMode==2)
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
					switchHeater(0); //关继电器，停止加热
					//sendDataToHost( temperatureX10, 0 );
				}
				else if( temperatureX10<=(gSetTemperatureX10-1) ) //低于设定值0.1度
				{
					switchHeater(1); //开继电器，开始加热
					//sendDataToHost( temperatureX10, 0 );
				}
			}
			
			//向主机发送数据
			if( gSendDataNow || ++timerCounter10s >= 60 )  //每60*10＝600秒发送一次数据
			{
				if( gSendDataNow )
				{
					gSendDataNow = 0;
					sendDataToHost( 51, readAm2321Result ); //立即状态报告 functionNumber=51
				}
				else
				{
					sendDataToHost( 1, readAm2321Result ); //定时状态报告 functionNumber=1
				}
				
				//timer清零
				timerCounter10s=0;
			}
		}
		else
		{
			//AM2321读取出错
			sendDataToHost( 9, readAm2321Result ); //错误、异常报告
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
	
	//*(receivedData+0) From Node ID
	//*(receivedData+1) Function
	
	switch( *(receivedData+1) )
	{
		case 20: //执行命令，无需返回
			
			//字节2，控制落地灯继电器工作模式（0：常关，1：常开）
			RELAY_LAMP = (*(receivedData+2)==0)?1:0;

			//字节3，控制油汀继电器的工作模式 （0：常关，1：常开，2：温控）
			gHeaterMode = *(receivedData+3);
			if( gHeaterMode == 0 ) switchHeater(0);
			else if( gHeaterMode == 1 ) switchHeater(1);

			//字节2，设定温度 10倍数
			//只能设定0-255，即0度至25.5度，之间的值
			//考虑到本节点只用于冬天控制油汀，这个设定温度区间应该是合理的
			gSetTemperatureX10 = *(receivedData+4);
			break;


		case 50: //请求立即报告状态
			//回报50, functionNum = 51;
			gSendDataNow = 1;
		
			break;
	
	}//end of switch
}
