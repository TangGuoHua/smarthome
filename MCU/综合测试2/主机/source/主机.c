/*
All rights are reserved.
Please contact huangchanghao@gmail.com if any questions.

When        Who        Remarks
--------------------------------------
2011-OCT-05 Changhao   Initial version
*/


#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "delay.h"
#include "serialPort.c"

sbit SPEAKER=P3^6;  //喇叭




void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void beep()
{
	uint i;
	
	for(i=0; i<15; i++)
	{
		SPEAKER = 1;
		delay(2);
		SPEAKER=0;
		delay(4);
	}
}


void sendDataToNode( byte* txData )
{
	//byte txData[2];
	byte sendRFChannel;
	byte sendAddr[ADDRESS_WIDTH]; //= { 0x61, 0x53, 0x95 }; //从节点地址
	byte sendDataLen;

	sendRFChannel = *(txData+2);
	
	sendAddr[0] = *(txData+3);
	sendAddr[1] = *(txData+4);
	sendAddr[2] = *(txData+5);
	
	sendDataLen = *(txData+6);
	
	//txData[0] = 2;
	//txData[1] = 2;
	
	//buffer[0]=20;
	//buffer[1]=20;
	//serialPortSendByte( *(txData) );
	//serialPortSendByte( *(txData+1));

		
	//nrfSendData( sendRFChannel, ADDRESS_WIDTH, sendAddr, 2, txData );//发送
	//serialPortSendByte( 99 );
	nrfSendData( sendRFChannel, ADDRESS_WIDTH, sendAddr, sendDataLen, (txData+7) );//发送	
	

}




/************************主函数********************/
void main()
{
	initINT0(); //初始化中断0 供24L01使用
	
	serialPortInit();//串口初始化
	
	nrf24L01Init();
	
	nrfSetRxMode();//设置为接收模式
	
	while(1)
	{
		sendDataToNode( serialPortReceive() );
		beep();
		nrfSetRxMode();//设置为接收模式
		P1=0xff;
	}
}



void Nrf24L01Interrupt(void) interrupt 0
{
	byte* receivedData;

	receivedData = nrfGetReceivedData();

	
	serialPortSendFrame( *receivedData++, *receivedData++, receivedData );
//	
//	serialPortSendByte( MCU2PC_FRAME_HEADER0 );
//	serialPortSendByte( MCU2PC_FRAME_HEADER1 );
//	serialPortSendByte( MCU2PC_FRAME_HEADER2 );
//	serialPortSendByte( MCU2PC_FRAME_HEADER3 );
//
//
//	serialPortSendByte( *receivedData++ );
//	serialPortSendByte( *receivedData++ );
//	serialPortSendByte( *receivedData++ );
//	serialPortSendByte( *receivedData++ );
//	serialPortSendByte( *receivedData++ );
//	serialPortSendByte( *receivedData++ );
//	serialPortSendByte( *receivedData++ );
//	
//	serialPortSendByte( MCU2PC_FRAME_TAILER0 );
//	serialPortSendByte( MCU2PC_FRAME_TAILER1 );
//	serialPortSendByte( MCU2PC_FRAME_TAILER2 );
		
	P1=0x0f;
	beep();
	P1=0xff;

}


void UART_SER (void) interrupt 4
{
	unsigned char temp;
	
	if(RI)
	{
		RI=0;
		temp=SBUF;
		
		if(temp=='s')
		{
			beep();
		}
	}
	if(TI)
	{
		TI=0;
		P1=0xf0;
		
	}
} 