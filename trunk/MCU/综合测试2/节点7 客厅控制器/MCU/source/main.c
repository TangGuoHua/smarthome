// STC11F04E 1T单片机  内部RC振荡

#include <reg52.h>
#include <intrins.h>

#include "nrf24L01Node.h"
#include "am2301.h"

sfr AUXR   = 0x8E;

sbit SENSOR=P1^7;  // 热释电红外线传感器 PIR
sbit RELAY_1=P3^7; //继电器1和2
sbit RELAY_2=P1^1;
sbit POWER_METER = P1^0; //电能表模块脉冲输入 1600个脉冲为1度电

//中断计数器，每50ms加一
unsigned char timerMSCounter = 0;

//中断计数器，每1秒加一
unsigned int timerSecCounter = 0;

//自主发送数据间隔
unsigned int sendDataInterval = 300;
bit sendDataFlag = 0;

//电能表脉冲计数
union { 
	unsigned long longVal;
	unsigned char byteVal[4];
	} powerMeterCount;


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}

void initTimer0(void)
{
	TMOD &= 0xf0; //清除定时器0模式位
	TMOD |= 0x01; //设定定时器0为方式1：16位计数器


	//TH0=0x8f; //给定时器赋初值，定时时间50ms，内部RC振荡
	//TL0=0x00; 
	
	TH0 = 0x77; // 5ms
    TL0 = 0x48;

	ET0=1; //允许定时 0 中断 
	EA=1; //允许总中断 
	TR0=1; //启动定时器 0 
	
	//清计数器
	timerMSCounter = 0;
	timerSecCounter = 0;
	

}

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=530; i>0; i-- ); // 1T STC11F04E 内部RC 6.97MHz
}


//通过24L01将AM2301数据发送给主机
void sendAM2301Data()
{
	unsigned char txData[HOST_DATA_WIDTH];
	
	txData[0] = NODE_ID_0; //Node ID0
	txData[1] = NODE_ID_1; //Node ID1
	
	txData[2] = readAM2301(2);
	txData[3] = getAM2301Data(0);
	txData[4] = getAM2301Data(1);
	txData[5] = getAM2301Data(2);
	txData[6] = getAM2301Data(3);
	txData[7] = getAM2301Data(4);
	txData[8] = powerMeterCount.byteVal[0];
	txData[9] = powerMeterCount.byteVal[1];
	txData[10] = powerMeterCount.byteVal[2];
	txData[11] = powerMeterCount.byteVal[3];
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
}

//void sendPIRData()
//{
//	unsigned char txData[HOST_DATA_WIDTH];
//
//	txData[0] = NODE_ID_0; //Node ID0
//	txData[1] = NODE_ID_1; //Node ID1
//	
//	txData[2] = 5;
//	txData[3] = 6;
//	txData[4] = 7;
//	txData[5] = 8;
//	txData[6] = 9;
//		
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
//
//}

void main( void )
{
	//PIR sensor状态变量
	//bit thisSensor=0, lastSensor=0;
	
	// 电能表模块脉冲状态变量
	bit thisPowerMeter=1, lastPowerMeter=1;
	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	
	//关断两个继电器
	RELAY_1 = 1;
	RELAY_2 = 1;
	
	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时3秒，让大部分元件稳定下来
	delayMS( 3000 );
	
	// initialize AM2301
	initAM2301();
	
	//初始化中断0
	initINT0();
		
	//initialize 24L01
	nrf24L01Init();
	//设置24L01为接收模式PRX
	nrfSetRxMode();
	
	//初始化定时器0
	initTimer0();
	
	//SENSOR = 0;
	
	// 电量计数器清零
	powerMeterCount.longVal = 0;
	
	while(1)
	{
//		sendData(); 
//		delayMS(60000);
		
//		thisSensor = SENSOR;
//		if( lastSensor!= thisSensor && thisSensor ==1 )
//		{
//			sendPIRData();
//			
//			//设置24L01为接收模式PRX
//			nrfSetRxMode();
//		}
//		lastSensor = thisSensor;

		thisPowerMeter = POWER_METER;
		if( thisPowerMeter != lastPowerMeter )
		{
			if( thisPowerMeter == 0 ) //低电平脉冲
			{
				powerMeterCount.longVal++;
			}
			lastPowerMeter = thisPowerMeter;
		}
		
		if( sendDataFlag )
		{
			//清标识位
			sendDataFlag = 0;
			
			//发送温湿度数据给主机
			sendAM2301Data();
			
			//设置24L01为接收模式PRX
			nrfSetRxMode();
		}
				
		
	}
}

//定时器0中断处理程序
void interruptTimer0(void) interrupt 1
{
	TR0=0; //关闭定时器0
	ET0=0; //关闭定时器0中断
	//TH0=0x8f; //给定时器0重赋初值，定时时间50ms，内部RC振荡
	//TL0=0x00; //重装低8位（修正后的值） 
	//1f慢，22慢，24慢，30慢，40快，36快，32快，31慢，32慢，36慢，40慢
	
	TH0 = 0x77; //0x76
    TL0 = 0x48;	//0x48
	

	//TR0=1; //打开定时器0	

	if( ++timerMSCounter==200 ) //每200次中断是1秒，即，5ms*20=1000ms
	{
		timerMSCounter=0;
		if( ++timerSecCounter == sendDataInterval )
		{
			timerSecCounter = 0;
			sendDataFlag = 1;
		}
	}
	ET0=1; //打开定时器0中断
	TR0=1; //打开定时器0


}

//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	receivedData = nrfGetReceivedData();
		
	RELAY_1 = *(receivedData++)==0?1:0;
	RELAY_2 = *(receivedData++)==0?1:0;
}