/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年08月20日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。

编译命令行 gcc -Wall -lwiringPi -o "main" "main.c" "nrf24L01.c"

*****************************************************************************/


#include <stdio.h>
#include <unistd.h>
//#include <wiringPi.h>
#include "nrf24L01.h"


void sendDataToHost( )
{
	unsigned char sendData[4];
	unsigned char toAddr[3]= {97, 83, 41}; //Pi's address is {53, 69, 149};
	unsigned char tmp;

		
	sendData[0] = 1;
	sendData[1] = 0;
	sendData[2] = 0;
	sendData[3] = 0; 


	//tmp = nrfSendData( 96, 3, toAddr, 16, sendData);
	tmp = nrfSendData( 92, 3, toAddr, 4, sendData);
	
	printf( "Send Data Result=%d\n\r\n\r\n\r", tmp);
	

}

void sendDataToPi( )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149};
	unsigned char tmp;

		
	sendData[0] = 88;
	sendData[1] = 0;
	sendData[2] = 88;
	sendData[3] = 0; 


	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);
	//tmp = nrfSendData( 92, 3, toAddr, 4, sendData);
	
	printf( "Send to Pi Result=%d\n\r\n\r\n\r", tmp);
	

}

void sendDataToPi3( int cnt )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {53, 69, 149};
	unsigned char tmp;

		
	sendData[0] = 88;
	sendData[1] = cnt;
	sendData[2] = 88;
	sendData[3] = 0; 


	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);
	//tmp = nrfSendData( 92, 3, toAddr, 4, sendData);
	
	printf( "Send to Pi Result=%d\n\r\n\r\n\r", tmp);
	

}

void sendDataToPi2( )
{
	unsigned char sendData[16];
	unsigned char toAddr[3]= {153, 169, 49};
	unsigned char tmp;

		
	sendData[0] = 88;
	sendData[1] = 0;
	sendData[2] = 88;
	sendData[3] = 0; 


	tmp = nrfSendData( 66, 3, toAddr, 16, sendData);
	//tmp = nrfSendData( 92, 3, toAddr, 4, sendData);
	
	printf( "Send to Pi2 Result=%d\n\r\n\r\n\r", tmp);
	

}

void startRecv( void )
{
	unsigned char myAddr[3] = {53,70, 132};
	nrfSetRxMode( 99, 3, myAddr );
	
	printf( "Starting listening... \n\r" );
	
}

int main(int argc, char **argv)
{
	unsigned char *receivedData;
	unsigned int cnt = 0;
	
	printf( "Test NRF24L01+ ... \n\r" );
	
	nrf24L01Init();
	
	sendDataToHost();
	
	sendDataToHost();
	
	sendDataToPi();
	
	sendDataToPi2();
	
	startRecv();
	
	while(1)
	{
		if( nrfIsDataReceived() )
		{
			printf( "Data Received. [%d]", ++cnt );
			receivedData = nrfGetReceivedData();
			printf( "D1=%d, D2=%d, D3=%d, D4=%d, D5=%d D6=%d\n\r", *(receivedData++), *(receivedData++),*(receivedData++),*(receivedData++),*(receivedData++),*(receivedData++) );
			
			if( cnt%5==0)
			{
				sendDataToPi3(cnt);
				startRecv();
			}
			
		}
			
		usleep(10000);
	}

	return 0;

}

