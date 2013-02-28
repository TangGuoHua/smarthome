/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include <reg52.h>
#include "dataType.h"
#include "nrf24L01Node.h"
#include "delay.h"

sbit RELAY=P3^7;  // �̵���

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
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
//
//}				

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
	// �ض�
	RELAY=1;
	
	//��ʼ���ж�0
	initINT0();

	
	//��ʼ��24L01
	nrf24L01Init();
	
	//����24L01Ϊ����ģʽPRX
	nrfSetRxMode();

	

	while(1)
	{

		//RELAY = ~RELAY;
		//delay(2000);
	  
	}
}





//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	byte* receivedData;

	
	receivedData = nrfGetReceivedData();

	//���յ�0��ر�LED����0��򿪡�
	RELAY = (*receivedData==0)?1:0;
		
	//cmd = *receivedData++ ;
	//param = *receivedData++;
	

	//�ٴν������ģʽ
	nrfSetRxMode();

}
