/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "delay.h"

sbit RELAY=P3^7;  // 继电器

//void sendData()
//{
//	byte txData[HOST_DATA_WIDTH];
//
//	
//	txData[0] = 100; //Node ID0
//	txData[1] = 2; //Node ID1
//	txData[2] = 5;
//	txData[3] = 6;
//	txData[4] = 7;
//	txData[5] = 8;
//	txData[6] = 9;
//		
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送
//
//}				

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
	// 关断
	RELAY=1;
	
	//初始化中断0
	initINT0();

	
	//初始化24L01
	nrf24L01Init();
	
	//设置24L01为接收模式PRX
	nrfSetRxMode();

	

	while(1)
	{

		//RELAY = ~RELAY;
		//delay(2000);
	  
	}
}





//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	byte* receivedData;

	
	receivedData = nrfGetReceivedData();

	//接收到0则关闭LED，非0则打开。
	RELAY = (*receivedData==0)?1:0;
		
	//cmd = *receivedData++ ;
	//param = *receivedData++;
	

	//再次进入接收模式
	nrfSetRxMode();

}
