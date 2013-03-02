/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
2013-MAR-02 Changhao   增加自动红外感应控制，增加向主机发送有无人状态的功能
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "delay.h"

sbit RELAY=P3^7;  // 继电器
sbit PIR=P3^4; //红外探头

unsigned char workMode = 0; //工作模式 0：常关，1：常开，2：自动感应

void sendData( bit thisPIR )
{
	byte txData[HOST_DATA_WIDTH];

	
	txData[0] = 100; //Node ID0
	txData[1] = 3; //Node ID1
	txData[2] = thisPIR;

		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
	
	//再次进入接收模式
	nrfSetRxMode();
}				

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main()
{
	bit thisPIR, lastPIR;

	// 关断
	RELAY=1;
	
	//初始化中断0
	initINT0();

	
	//初始化24L01
	nrf24L01Init();
	
	//设置24L01为接收模式PRX
	nrfSetRxMode();

	thisPIR = PIR;
	lastPIR = thisPIR;

	while(1)
	{
		//读取红外传感器输出状态
		thisPIR = PIR;
		
		if( thisPIR != lastPIR )
		{
			//如果工作在自动感应模式，则控制继电器
			if( workMode ==2 )
				RELAY = ~thisPIR;
				
			//发送餐厅有无人的信息给主机
			sendData( thisPIR );
			
			lastPIR = thisPIR;
		}
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	byte* receivedData;
	
	receivedData = nrfGetReceivedData();

	//设置工作模式
	workMode = *receivedData;
	if( workMode==0 )
	{
		//off
		RELAY=1;
	}
	else if( workMode==1 )
	{
		//on
		RELAY=0;
	}
	
	//再次进入接收模式
	nrfSetRxMode();
}