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


void testChannel( unsigned char rfChannel )
{
	unsigned char sendData[10];
	unsigned char toAddr[3]= {97,83,175};
	unsigned char tmp;

		
	sendData[0] = 2; //From
	sendData[1] = 1; //function
	sendData[2] = rfChannel; //channel
	sendData[3] = 100; //qty


	tmp = nrfSendData( 125, 3, toAddr, 10, sendData);
	
	printf( "\n\rTesting Channel [%d]. Trigger sent. Result=%d\n\r", rfChannel, tmp);
	

}

void startRecv( unsigned char rfChannel )
{
	unsigned char myAddr[3] = {53,70, 132};
	nrfSetRxMode( rfChannel, 3, myAddr );
	
	printf( "Starting listening on channel [%d]... \n\r" , rfChannel);
	
}

int main(int argc, char **argv)
{
	unsigned char *receivedData;
	unsigned char cnt = 0;
	
	unsigned int timeout=10000;
	
	unsigned char rfChannel=0;
	
	printf( "Test NRF24L01+ ... \n\r" );
	
	nrf24L01Init();
	
	for( ;rfChannel<=124; rfChannel++)
	{


		testChannel( rfChannel);
		
		startRecv(rfChannel);
		timeout=10000;
		cnt=0;
		while(--timeout>10)
		{
			if( nrfIsDataReceived() )
			{
				printf( "Channel [%03d] received data #[%3d] - ", rfChannel, ++cnt );
				receivedData = nrfGetReceivedData();
				receivedData++; //skip Sender
				receivedData++; //skip Function #
				printf( "SN:%3d, Last ReTxn:%2d, ReTxn0:%2d, ReTxn5:%2d, ReTxn10:%2d, ReTxn15:%2d, TxFailed:%2d\n\r", *(receivedData++),*(receivedData++),*(receivedData++),*(receivedData++), *(receivedData++), *(receivedData++), *(receivedData++) );
				timeout=10000;
			}
				
			usleep(1);
		}
	}


	return 0;

}

