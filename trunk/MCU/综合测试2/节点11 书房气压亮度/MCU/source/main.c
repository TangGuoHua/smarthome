//***************************************
// BMP085 IIC测试程序
// 使用单片机11F04E
// 时间：2012年9月17日
//****************************************
#include <reg52.h>	 
#include "bmp085.h" 
#include "bh1750fvi.h"
#include "nrf24L01.h"
#include "nrf24L01Node.h"

typedef union
{
	long val;
	unsigned char byte[4];
} MY_LONG;

sfr AUXR = 0x8E;

sbit PIR=P1^7; //热释红外探头引脚定义

//计时中断累计10ms和1s的变量
volatile unsigned char count10ms = 0;
volatile unsigned int count1s = 0;

//时间到发送数据的标志位
volatile bit toSend = 0;

void initTimer0(void)
{
	TMOD = 0x01;
    TH0 = 0x10;
    TL0 = 0x30;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void main()
{ 
	unsigned char sendData[15];
	//unsigned char addr[3];
	bit thisPIR=0, lastPIR=0;
	MY_LONG tmp;

	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	bmp085Init(); //初始化BMP085 
	bh1750Init(); // 初始化 BH1750FVI
	
	initINT0(); //初始化中断0
	nrf24L01Init(); //初始化NRF24L01+
	//nrfSetRxMode(); //设置nrf24L01为接收模式
	
	initTimer0(); //初始化Timer0
	
	while(1) //循环
	{
		//获取红外探头当前状态
		thisPIR = PIR;
		
//		//发送红外探头数据 100-111
//		if( thisPIR != lastPIR)
//		{
//			lastPIR = thisPIR;
//			
//			//向主机传送数据
//			sendData[0] = NODE_ID_0;
//			sendData[1] = 111;
//			sendData[2] = thisPIR;
//			nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
//			
//			//向报警器送数据
//			sendData[0] = 111;
//			sendData[1] = thisPIR;
//			addr[0]=97;
//			addr[1]=83;
//			addr[2]=170;
//			nrfSendData( 82, ADDRESS_WIDTH, addr, 2, sendData);
//
//			
//			//书房灯控制
//			if( thisPIR && bh1750GetBrightness()<1)
//			{
//				//开灯
//				sendData[0] = 111;
//				sendData[1] = 1;
//				addr[0]=97;
//				addr[1]=83;
//				addr[2]=175;
//				nrfSendData( 82, ADDRESS_WIDTH, addr, 2, sendData);
//			}
//			else
//			{
//				//关灯
//				sendData[0] = 111;
//				sendData[1] = 0;
//				addr[0]=97;
//				addr[1]=83;
//				addr[2]=175;
//				nrfSendData( 82, ADDRESS_WIDTH, addr, 2, sendData);
//			}
//
//			//向报警器节点传送数据
//			//nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
//				
//			//nrfSetRxMode(); //设置nrf24L01为接收模式
//
//		}
		
		//定时发送气压温度亮度等数据 100-11
		if( toSend )
		{
			// initialize Node ID
			sendData[0]=NODE_ID_0;
			sendData[1]=NODE_ID_1;
			
			// 发送气压、温度、亮度数据
			bmp085Convert();
			
			tmp.val = bmp085GetTemperature();
			sendData[2]=tmp.byte[0];
			sendData[3]=tmp.byte[1];
			sendData[4]=tmp.byte[2];
			sendData[5]=tmp.byte[3];
			// 数据复原 tmp.byte[0]*16777216 + tmp.byte[1]*65536 + tmp.byte[2]*256 + tmp.byte[3]
			
			tmp.val = bmp085GetPressure();
			sendData[6]=tmp.byte[0];
			sendData[7]=tmp.byte[1];
			sendData[8]=tmp.byte[2];
			sendData[9]=tmp.byte[3];
			// 数据复原 tmp.byte[0]*16777216 + tmp.byte[1]*65536 + tmp.byte[2]*256 + tmp.byte[3]
			
			tmp.val = (unsigned long) bh1750GetBrightness();
			sendData[10]=tmp.byte[0];
			sendData[11]=tmp.byte[1];
			sendData[12]=tmp.byte[2];
			sendData[13]=tmp.byte[3];
			// 数据复原 tmp.byte[2]*256 + tmp.byte[3]
			
			sendData[14] = thisPIR;
			
			nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, sendData);
			
			toSend = 0;
			
			//nrfSetRxMode(); //设置nrf24L01为接收模式
		}
	}
}

// 中断每10ms一次
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x1d;
    TL0 = 0x70;
    
	if( ++count10ms == 100 ) // 每100个10ms就是1秒
	{
		TR0=0;
		count10ms = 0;
		if( ++count1s == 600 )
		{
			count1s = 0;
			toSend = 1;
		}
		TR0=1;
	}
}


////外部中断0处理程序
////用于处理来自24L01的中断
//void interrupt24L01(void) interrupt 0
//{
//	unsigned char * receivedData;
//	
//	//暂停计时（防止中断混乱）
//	TR0 = 0;
//
//	receivedData = nrfGetReceivedData();
//	
//	modeLight = *(receivedData++);
//	modeAlarm = *(receivedData++);
//	
//	switch( modeLight)
//	{
//		case 0: //关
//			controlLight(0);
//			break;
//		
//		case 1: //开
//			controlLight(1);
//			break;	
//		
//		case 2: //自动
//			if( PIR && (bh1750GetBrightness()<1) ) //有人且亮度小于1才开灯
//			{
//				controlLight(1);
//			}
//			else
//			{
//				controlLight(0);
//			}
//	}
//
//	//报警器模式设为0则马上关闭报警器
//	if( modeAlarm==0 )
//	{
//		controlAlarm(0);
//	}
//	//toSend = 1;
//	
//	nrfSetRxMode(); //设置nrf24L01为接收模式
//
//	
//	//继续计时
//	TR0 = 1;
//}