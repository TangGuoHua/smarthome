/* STC11F04E, 1T, 内部RC振荡*/

#include<reg52.h>
#include "nrf24L01Node.h"

sbit OUT_RELAY=P1^6;  //继电器控制端 1-off, 0-on
unsigned char lightMode=0; //灯工作模式 0－关、1－开、2－自动

void delay(void)   //误差 -0.000000000006us
{
    unsigned char a,b,c;
    for(c=26;c>0;c--)  //c=26 约1秒
        for(b=250;b>0;b--)
            for(a=250;a>0;a--);
}

//void sendData( unsigned char onOff)
//{
//	unsigned char txData[HOST_DATA_WIDTH];
//	txData[0] = onOff; 
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
//
//}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


main()
{
	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时200ms，让大部分元件稳定下来
	delay();
	
	OUT_RELAY = 1;
		
	//初始化中断0
	initINT0();
	
	nrf24L01Init();
	
	nrfSetRxMode(); //开始接收
	
	while(1)
	{

	}
}

//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	unsigned char  senderID, param;
	
	receivedData = nrfGetReceivedData();
	
	//本节点接收2个字节
	//字节1：发送者ID  （senderID＝0为Pi，即主节点；senderID＝111为书房控制器节点）
	//字节2：参数 （解释如下：）
	//       0节点发送的参数为工作模式， 0－关、1－开、2－自动
	//       111节点发送的参数为开关灯信号， 0－关、1－开 （仅在工作模式为2时有效）
	senderID = *receivedData++;
	param = *receivedData;
	
	if( senderID == 0 ) //主节点发来的数据
	{
		lightMode=param;
		if( lightMode == 0 )
		{
			OUT_RELAY=1;
		}
		else if( lightMode == 1 )
		{
			OUT_RELAY=0;
		}
	}
	else if( senderID==111 ) //书房控制器发来的数据
	{
		if( lightMode == 2 )
		{
			//接收到0则关灯，非0则打开。
			OUT_RELAY = (param==0)?1:0;
		}
	}

	//再次进入接收模式
	//nrfSetRxMode();
}