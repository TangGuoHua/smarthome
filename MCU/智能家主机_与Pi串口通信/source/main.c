/********************************************************
1T STC11F04E 3.6864MHz
PI�뵥Ƭ������ͨ��

���߱���һ��Ȩ����������������ϵhuangchanghao@gmail.com

All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

Revision History
Date         Author      Remarks
-----------------------------------------------------------
2013-FEB-27  Changhao    ȥ����blinkLED ������û�����뵥Ƭ����
2013-SEP-30  Changhao    ���ڲ�������ߵ�115200bps
                         �޸�ͨѶЭ�飺�������ڵ�̶�ÿ֡����16�ֽڣ�����1���ֽڵ�NodeID��15���ֽڵ����ݡ�

**********************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "serialPort.c"

//Header0��Header1���Ϊһ����ֵ��������ǿϵͳ���ݴ���
//������μ���λ����MCUCommunicator�������ע��

//MCU to PC
//BA9876543210
#define MCU2PC_FRAME_HEADER0 0xba
#define MCU2PC_FRAME_HEADER1 0xba
#define MCU2PC_FRAME_HEADER2 0x98
#define MCU2PC_FRAME_HEADER3 0x76

#define MCU2PC_FRAME_TAILER0 0x54
#define MCU2PC_FRAME_TAILER1 0x32
#define MCU2PC_FRAME_TAILER2 0x10


//sfr AUXR   = 0x8E;
sbit LED1=P1^7;  //Blue LED
sbit LED2=P1^6;  //Red LED

//��ʼ���ж�0����NRF24L01��
void initINT0(void)
{
	EA=1; //�����ж�
	EX0=1; // Enable int0 interrupt.
}

// nrf24L01 send
void sendDataToNode( unsigned char* txData )
{
	//byte txData[2];
	unsigned char sendRFChannel;
	unsigned char sendAddr[ADDRESS_WIDTH];
	unsigned char sendDataLen;

	sendRFChannel = *(txData);
	
	sendAddr[0] = *(txData+1);
	sendAddr[1] = *(txData+2);
	sendAddr[2] = *(txData+3);
	
	sendDataLen = *(txData+4);
	
	nrfSendData( sendRFChannel, ADDRESS_WIDTH, sendAddr, sendDataLen, (txData+5) );//����	
}


void delay50ms(void)   //��� -0.000000000001us
{
    unsigned char a,b;
    for(b=221;b>0;b--)
        for(a=207;a>0;a--);
}

void blinkLED( unsigned char c)
{
	if(c==1)
	{
		LED1=0;
		delay50ms();
		LED1=1;
	}
	else
	{
		LED2=0;
		delay50ms();
		LED2=1;
	}
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {53, 69, 149 }; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 96, 3, myAddr);  //96Ƶ����3�ֽڿ�ȵĵ�ַ
}

void main(void)
{
	
	delay50ms(); // ϵͳ�ϵ���ȶ�һ��
	delay50ms();
	
	LED1=1;
	LED2=1;
	
    //AUXR = AUXR|0x40;  // T1, 1T Mode
    serialPortInit();

	//��ʼ���ж�0
	initINT0();
	
	nrf24L01Init();
	
	startRecv(); //��ʼ����
		
	while(1)
	{
		sendDataToNode( serialPortReceive() );
		blinkLED(2);  //blink RED LED

		startRecv();//����Ϊ����ģʽ

	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	
	blinkLED(1);  //blink Blue LED
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	// ��������֡��ͨ�����ڷ�������
	serialPortSendFrame( *receivedData++, receivedData );

	//�ٴν������ģʽ
	//startRecv();
}
