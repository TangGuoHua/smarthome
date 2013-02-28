// STC12C5608AD 1T单片机  4M外部晶体振荡
// 遥控开关，用于遥控餐厅顶灯开

#include<reg52.h>
#include "nrf24L01Node.h"

sbit LED=P2^1; 

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M晶振
}



void sendData( unsigned char onOff)
{
	unsigned char txData[HOST_DATA_WIDTH];

	
	txData[0] = onOff; 

	

	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送

}

void main( void )
{

	//系统上电后，很多元件需要些时间稳定下来
	//故这里延时200ms，让大部分元件稳定下来
	delayMS( 200 );
	
	nrf24L01Init();
	
	LED=0;
	
	
	while(1)
	{
		LED = !LED;

		//发送数据给灯节点
		sendData( 1 );
		delayMS(500);
	}
}

