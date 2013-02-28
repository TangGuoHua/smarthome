/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
2011-OCT-07 Changhao   增加interval记入STC片内EEPROM的部分，保证下次开机后之前的设置不会丢失
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "dht11.h"
#include "ds18B20.h"
#include "delay.h"
#include "stcEEPROM.h"


//中断计数器，每50ms加一
byte timerMSCounter = 0;

//中断计数器，每1秒加一
uint timerSecCounter = 0;

//自主发送数据间隔
uint sendDataInterval = 10;
bit sendDataFlag = 0;


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}

void initTimer0(void)
{
	TMOD &= 0xf0; //清除定时器0模式位
	TMOD |= 0x01; //设定定时器0为方式1：16位计数器


	TH0=0x4c; //给定时器赋初值，定时时间50ms，11.0592MHz晶振
	TL0=0x00; 

	ET0=1; //允许定时 0 中断 
	EA=1; //允许总中断 
	TR0=1; //启动定时器 0 
	
	//清计数器
	timerMSCounter = 0;
	timerSecCounter = 0;
	

}

//将温湿度数据发送给主机
void sendHTData()
{
	byte txData[HOST_DATA_WIDTH];
	float temperature;
	int intTemperature;

	byte humidity;
	
	humidity = getHumidity(1);
	
	temperature = readTemperature(1);
	
	//四舍五入到十分位
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 

	txData[0] = 100; //Node ID0
	txData[1] = 1; //Node ID1
	txData[2] = humidity;
	txData[3] = temperature<0 ? 1 : 0; //如果是零下则此位置1
	txData[4] = intTemperature/10; //温度的整数部分
	txData[5] = intTemperature%10; //温度的小数部分
	txData[6] = 0;
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送

}

void serialPortInit(void)
{

	//28800bps@11.0592MHz
	PCON &= 0x7f; //波特率不倍速
	SCON = 0x50;  //8位数据,可变波特率 方式1
	REN = 0;      //不允许接收
	TMOD &= 0x0f; //清除定时器1模式位
	TMOD |= 0x20; //设定定时器1为8位自动重装方式
	TL1 = 0xFF;   //设定定时初值
	TH1 = 0xFF;   //设定定时器重装值
	ET1 = 0;      //禁止定时器1中断
	//EA = 1;       //打开总中断
	//ES = 1;       //打开串口中断
	ES=0;          //关闭串口中断
	TR1 = 1;      //启动定时器1
}


/*-----------
发送一个字节
-----------*/
void serialPortSendByte( byte dat )
{
	//ES=0; //关闭串口中断
	SBUF = dat;
	while(!TI); //等待发送完毕
	TI = 0;  //清发送完毕标志
	//ES=1;  //开启串口中断
}

//将模式号存入片内eeprom
void saveMode( byte mode )
{
	eepromEraseSector( STC_EEPROM_START_ADDR );
	eepromWrite( STC_EEPROM_START_ADDR, mode );
}

//从片内eeprom取得mode号
byte getMode()
{
	return eepromRead(STC_EEPROM_START_ADDR);
}


//根据模式号返回对应的发送数据时间间隔（秒）
uint getIntervalByMode( byte mode )
{
	uint interval;
	
	switch( mode )
	{
		case 0:
			interval = 0; //节点不自动返回温湿度值，靠主机发送读取命令读取
			break;
			
		case 1:
			interval = 5; //5sec
			break;

		case 2:
			interval = 10; //10sec
			break;

		case 3:
			interval = 15; //15sec
			break;
			
		case 4:
			interval = 20; //20sec
			break;

		case 5:
			interval = 30; //30sec
			break;

		case 6:
			interval = 45; //45sec
			break;
			
		case 7:
			interval = 60; //1min
			break;

		case 8:
			interval = 90; //1.5min
			break;
			
		case 9:
			interval = 120; //2min
			break;
			
		case 10:
			interval = 180; //3min
			break;

		case 11:
			interval = 300; //5min
			break;
			
		case 12:
			interval = 600; //10min
			break;

		case 13:
			interval = 900; //15min
			break;

		case 14:
			interval = 1200; //20min
			break;

		case 15:
			interval = 1800; //30min
			break;

		case 16:
			interval = 2700; //45min
			break;

		case 17:
			interval = 3600; //1hour
			break;

		case 18:
			interval = 5400; //1.5hours
			break;

		case 19:
			interval = 7200; //2hours
			break;

		case 20:
			interval = 10800; //3hours
			break;

		case 21:
			interval = 18000; //5hours
			break;

		case 22:
			interval = 21600; //6hours
			break;

		case 23:
			interval = 28800; //8hours
			break;

		case 24:
			interval = 36000; //10hours
			break;

		case 25:
			interval = 43200; //12hours
			break;

		case 26:
			interval = 57600; //16hours
			break;

		case 27:
			interval = 64800; //18hours
			break;
		
		//注意：sendDataInterval是无符号整型变量，最大值65535

		default:
			interval = 300; // 默认5分钟间隔
			break;

	} //swtich 结束
	return interval;
}


void setNewInterval( byte mode )
{
	//记下当前的interval，待会后面用于判断是否修改成功
	uint oldInterval = sendDataInterval;
	
	//得到新的interval
	sendDataInterval = getIntervalByMode( mode );
	
	if( sendDataInterval != oldInterval ) //说明interval变化了
	{

		//只要interval变化了，均要保存其到eeprom，否则下次开机后恢复默认值
		saveMode( mode );
		
		//如果新的interval不等于0，则重新初始化定时器0
		if( sendDataInterval !=0 )
		{
			//初始化定时器0
			//重新按新的interval开始计数
			initTimer0();
		}
		else
		{
			//interval为0意味着不用自主发送，仅供查询
			TR0=0; //关闭定时器0
		}
	}
}


//主函数
void main()
{
	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时3秒，让大部分元件稳定下来
	//delayMS( 2000 );
	
	//serialPortInit();
	
	//初始化中断0
	initINT0();

	//初始化DHT11
	initDHT11();
	
	//初始化24L01
	nrf24L01Init();
	
	//设置24L01为接收模式PRX
	nrfSetRxMode();
	
	//初始化定时器0
	initTimer0();
	
	//初始化Interval
	sendDataInterval = getIntervalByMode( getMode() );
	
	
	while(1)
	{
		if( sendDataFlag )
		{
			//清标识位
			sendDataFlag = 0;
			
			//发送温湿度数据给主机
			sendHTData();
			
			//设置24L01为接收模式PRX
			nrfSetRxMode();
		}


	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	byte* receivedData;
	byte cmd, param;
	
	TR0=0; //关闭定时器0
	
	receivedData = nrfGetReceivedData();
	cmd = *receivedData++ ;
	param = *receivedData++;
	
	serialPortSendByte( cmd );
	serialPortSendByte( param );
	
	switch(cmd)
	{
		case 80:
			//返回温湿度数据
			sendHTData();
			
			//清计时器
			//下次返回就是从现在开始计时的（sendDataInterval+探头采样时间）之后
			timerSecCounter = 0;
			break;
		
		case 90: //设置新的interval
			setNewInterval( param );
			break;
		
		default:
			break;	
	}
	
	//再次进入接收模式
	nrfSetRxMode();
	
	TR0=1; //打开定时器0
}

//定时器0中断处理程序
void interruptTimer0(void) interrupt 1
{
	TR0=0; //关闭定时器0
	ET0=0; //关闭定时器0中断
	TH0=0x4c; //给定时器0重赋初值，定时时间50ms，11.0592MHz晶振
	TL0=0x35; //重装低8位（修正后的值） 
	//1f慢，22慢，24慢，30慢，40快，36快，32快，31慢，32慢，36慢，40慢
	
	TR0=1; //打开定时器0

	if( ++timerMSCounter==20 ) //每20次中断是1秒，即，50ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter == sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //打开定时器0中断
}
