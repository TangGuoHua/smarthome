// STC12C5608AD 1T��Ƭ��  4M�ⲿ������
// ң�ؿ��أ�����ң�ز������ƿ�

#include<reg52.h>
#include "nrf24L01Node.h"

sbit LED=P2^1; 

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M����
}



void sendData( unsigned char onOff)
{
	unsigned char txData[HOST_DATA_WIDTH];

	
	txData[0] = onOff; 

	

	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����

}

void main( void )
{

	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ200ms���ô󲿷�Ԫ���ȶ�����
	delayMS( 200 );
	
	nrf24L01Init();
	
	LED=0;
	
	
	while(1)
	{
		LED = !LED;

		//�������ݸ��ƽڵ�
		sendData( 1 );
		delayMS(500);
	}
}

