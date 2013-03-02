/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
2013-MAR-02 Changhao   �����Զ������Ӧ���ƣ���������������������״̬�Ĺ���
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "delay.h"

sbit RELAY=P3^7;  // �̵���
sbit PIR=P3^4; //����̽ͷ

unsigned char workMode = 0; //����ģʽ 0�����أ�1��������2���Զ���Ӧ

void sendData( bit thisPIR )
{
	byte txData[HOST_DATA_WIDTH];

	
	txData[0] = 100; //Node ID0
	txData[1] = 3; //Node ID1
	txData[2] = thisPIR;

		
	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
	
	//�ٴν������ģʽ
	nrfSetRxMode();
}				

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


/*------------------------------------------------
                    ������
------------------------------------------------*/
void main()
{
	bit thisPIR, lastPIR;

	// �ض�
	RELAY=1;
	
	//��ʼ���ж�0
	initINT0();

	
	//��ʼ��24L01
	nrf24L01Init();
	
	//����24L01Ϊ����ģʽPRX
	nrfSetRxMode();

	thisPIR = PIR;
	lastPIR = thisPIR;

	while(1)
	{
		//��ȡ���⴫�������״̬
		thisPIR = PIR;
		
		if( thisPIR != lastPIR )
		{
			//����������Զ���Ӧģʽ������Ƽ̵���
			if( workMode ==2 )
				RELAY = ~thisPIR;
				
			//���Ͳ��������˵���Ϣ������
			sendData( thisPIR );
			
			lastPIR = thisPIR;
		}
	}
}



//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	byte* receivedData;
	
	receivedData = nrfGetReceivedData();

	//���ù���ģʽ
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
	
	//�ٴν������ģʽ
	nrfSetRxMode();
}