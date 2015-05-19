//STC12C5616AD 1T , 4MHz����

#include <reg52.h>
#include "nrf24L01Node.h"
#include <stdlib.h>  

sbit LED1 = P1^4;
sbit LED2 = P1^5;

volatile unsigned char replyFlag = 0;
volatile unsigned char channel;
volatile unsigned char qty;

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}

void delay500ms(void)   //��� 0us
{
    unsigned char a,b,c;
    for(c=46;c>0;c--)
        for(b=152;b>0;b--)
            for(a=70;a>0;a--);
}

void delay100us(void)   //��� 0us
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=97;a>0;a--);
}

void delay5s(void)   //11.0592Mhz
{
    unsigned char a,b,c,n;
    for(c=249;c>0;c--)
        for(b=219;b>0;b--)
            for(a=252;a>0;a--);
    for(n=32;n>0;n--);

}

//void delay10s(void) 
//{
//
//	//4Mhz Crystal, 1T STC11F04E
//    unsigned char a,b,c;
//    for(c=191;c>0;c--)
//        for(b=209;b>0;b--)
//            for(a=249;a>0;a--);
//
//}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97,83,175,231,231}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 125, 3, myAddr);
}




void replyData( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 70, 132, 231, 231}; //Pi, 3�ֽڵ�ַ
	
	//��һ֡���ݷ���ʱ��re-transmit�Ĵ�����
	//0:��ʾһ�η��ͳɹ���û��re-transmit
	//1-15:��ʾ�ط���1��15�γɹ�
	//100: ��֡Ϊ���͵ĵ�һ֡���ݣ�����û����һ�η��͵�re-transmit����
	//255: ����ʧ��
	unsigned char lastReTxn=100; 
	
	unsigned char cntReTxn0=0; //һ�η��ͳɹ����ط�0�Σ���֡��
	unsigned char cntReTxn5=0; //�ط�1��5�γɹ���֡��
	unsigned char cntReTxn10=0;//�ط�6��10�γɹ���֡��
	unsigned char cntReTxn15=0;//�ط�11��15�γɹ���֡��
	unsigned char cntFailed=0;//����ʧ�ܵ�֡��
	
	unsigned char c;
	
	
	sendData[0] = 210;//Node ID
	sendData[1] = 2;// reply to Function 1
	
	for( c=1;c<=(qty+1);c++ )
	{
		
		sendData[2] = c;
		sendData[3] = lastReTxn;
		sendData[4] = cntReTxn0;
		sendData[5] = cntReTxn5;
		sendData[6] = cntReTxn10;
		sendData[7] = cntReTxn15;
		sendData[8] = cntFailed;
	
		lastReTxn = nrfSendData(channel, 5, toAddr, 16, sendData);
		
		if( lastReTxn == 0 )
			cntReTxn0++;
		else if( 1<=lastReTxn && lastReTxn<=5 )
			cntReTxn5++;
		else if( 6<=lastReTxn && lastReTxn<=10 )
			cntReTxn10++;
		else if( 11<=lastReTxn && lastReTxn<=15 )
			cntReTxn15++;
		else if( lastReTxn == 255 )
			cntFailed++;

	}
	
	 
}

void sendData()
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231};
	
	sendData[0] = 251;
	sendData[1] = 1;
	sendData[2] = 2;
	sendData[3] = 3;
	sendData[4] = 4;
	sendData[5] = 5;
	
	nrfSendData(96, 5, toAddr, 16, sendData);

}


void main()
{

	

	//delay500ms();
	
	//RELAY=1; //switch off relay
	
	//��ʼ���ж�0
	//initINT0();
	
	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	//startRecv(); 
	

	while(1)
	{	
		sendData();
		delay5s();
	}
}



//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	channel = *(receivedData+2);
	qty = *(receivedData+3);
	
	replyFlag = 1;

}
