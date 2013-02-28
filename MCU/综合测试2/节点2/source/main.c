/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"


sbit SENSOR=P3^7;  // 热释电红外线传感器

void sendData()
{
	byte txData[HOST_DATA_WIDTH];

	
	txData[0] = 100; //Node ID0
	txData[1] = 2; //Node ID1
	txData[2] = 5;
	txData[3] = 6;
	txData[4] = 7;
	txData[5] = 8;
	txData[6] = 9;
		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//发送

}				

/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main()
{
	bit thisSensor, lastSensor;
	
	lastSensor = 0;
	thisSensor = 0;	
	
	//初始化24L01
	nrf24L01Init();
	

	while(1)
	{
		thisSensor = SENSOR;
		if( lastSensor!= thisSensor && thisSensor ==1 )
		{
			sendData();
		}
		lastSensor = thisSensor;
	  
	}
}




