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

#define SEND_RETRY 5

int testChannel( unsigned char rfChannel, unsigned char frameCount )
{
	unsigned char sendData[10];
	//unsigned char toAddr[5]= {97,83,175, 231, 231};
	unsigned char toAddr[3]= {97,83,175};
	unsigned char tmp;

		
	sendData[0] = 2; //From
	sendData[1] = 1; //function
	sendData[2] = rfChannel; //channel
	sendData[3] = frameCount; //qty


	tmp = nrfSendData( 125, 1, SEND_RETRY, 3, toAddr, 10, sendData);
	
	//printf( "\n\rTesting Channel [%d]. Trigger sent. Result=%d\n\r", rfChannel, tmp);
	printf( "\n\r%d,\t%d,", rfChannel, tmp);

	return tmp;

}

void startRecv( unsigned char rfChannel )
{
	unsigned char myAddr[5] = {53,70, 132, 231, 231};
	nrfSetRxMode( rfChannel, 5, myAddr );
	
	//printf( "Starting listening on channel [%d]... \n\r" , rfChannel);
	
}



int main(int argc, char **argv)
{
	unsigned char *receivedData;
	unsigned char cnt = 0;
	
	unsigned long timeout=60000;
	
	unsigned char rfChannel=0;
	
	unsigned char testFrameCount=200;
	
	//received statistics
	unsigned char rSN, rLastRT, rR0, rR5, rR10, rR15, rFail;

	//my statistics
	unsigned char mySN, myLastRT, myR0, myR5, myR10, myR15, myFail;
	
	//score
	unsigned int score;

	printf( "Start testing NRF24L01+ ... \n\r" );
	printf( "CH,\tTrg,\tMySN,\tScore\n\r");
	//printf( "--------------------------------------------------------------\n\r");
	
	// initialize
	nrf24L01Init();
	
	//testing channels
	for( rfChannel=80;rfChannel<=120; rfChannel++)
	{
		if( rfChannel==100 ) continue;


		timeout=120000;

		//reset statistics
		cnt=0;

		mySN=0;
		myLastRT=0;
		myR0=0;
		myR5=0;
		myR10=0;
		myR15=0;
		myFail=0;
		
		rSN=0;
		rLastRT=0;
		rR0=0;
		rR5=0;
		rR10=0;
		rR15=0;
		rFail=0;

		//trigger slave to send back data
		testChannel( rfChannel, testFrameCount) ;

		//start receiving
		startRecv(rfChannel);
			
		//printf( "Testing channel [%d]... \r\n", rfChannel );
		while(--timeout>10)
		{
			if( nrfIsDataReceived() )
			{
				
				
				mySN++;
				
				receivedData = nrfGetReceivedData();
				receivedData++; //skip Sender
				receivedData++; //skip Function #
				rSN = *(receivedData++);
				rLastRT = *(receivedData++);
				rR0 = *(receivedData++);
				rR5 = *(receivedData++);
				rR10 = *(receivedData++);
				rR15 = *(receivedData++);
				rFail = *(receivedData++);

				if( rLastRT==100 )
				{
					continue;
				}
				else if( rLastRT==0 )
					myR0++;
				else if( 1<= rLastRT && rLastRT <=5 )
					myR5++;
				else if( 6<= rLastRT && rLastRT <=10 )
					myR10++;
				else if( 11<= rLastRT && rLastRT <=15 )
					myR15++;
				else if( rLastRT==255 )
					myFail++;

				//printf( "Channel [%03d] received data #[%3d] - ", rfChannel, ++cnt );
				//printf( "SN:%3d, Last ReTxn:%2d, ReTxn0:%2d, ReTxn5:%2d, ReTxn10:%2d, ReTxn15:%2d, TxFailed:%2d\n\r", rSN, rLastRT, rR0, rR5, rR10, rR15, rFail);
				timeout=120000;
				if( cnt== (testFrameCount+1) || rSN== (testFrameCount+1) || (mySN + rFail) == (testFrameCount+1) )
				{
					usleep(200);
					break;
				}
			}
			
			usleep(10);
		}
		

//		if( mySN == (testFrameCount+1) )
//		{
			score= myR5*5+myR10*10+myR15*15+myFail*15 + (testFrameCount+1-mySN)*15;

			if( score>100 ) score=100;
			//printf( "Channel [%d], Score=[%d]\r\n" , rfChannel, score );
			//printf( "   SN:%3d, Last ReTxn:%2d, ReTxn0:%2d, ReTxn5:%2d, ReTxn10:%2d, ReTxn15:%2d, TxFailed:%2d\r\n", rSN, rLastRT, rR0, rR5, rR10, rR15, rFail);
			//printf( "MY SN:%3d, Last ReTxn:%2d, ReTxn0:%2d, ReTxn5:%2d, ReTxn10:%2d, ReTxn15:%2d, TxFailed:%2d\r\n", mySN, myLastRT, myR0, myR5, myR10, myR15, myFail);
			printf( "\t%3d,\t%d", mySN, score);
			
//		}
	}

	printf( "\n\r\n\r" );


	return 0;

}

