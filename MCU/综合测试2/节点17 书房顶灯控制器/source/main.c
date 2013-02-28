/* STC11F04E, 1T, �ڲ�RC��*/

#include<reg52.h>
#include "nrf24L01Node.h"

sbit OUT_RELAY=P1^6;  //�̵������ƶ� 1-off, 0-on
unsigned char lightMode=0; //�ƹ���ģʽ 0���ء�1������2���Զ�

void delay(void)   //��� -0.000000000006us
{
    unsigned char a,b,c;
    for(c=26;c>0;c--)  //c=26 Լ1��
        for(b=250;b>0;b--)
            for(a=250;a>0;a--);
}

//void sendData( unsigned char onOff)
//{
//	unsigned char txData[HOST_DATA_WIDTH];
//	txData[0] = onOff; 
//	nrfSendData( HOST_RF_CHANNEL, ADDRESS_WIDTH, HOST_ADDR, HOST_DATA_WIDTH, txData);//����
//
//}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


main()
{
	//ϵͳ�ϵ�󣬺ܶ�Ԫ����ҪЩʱ���ȶ�����
	//��������ʱ200ms���ô󲿷�Ԫ���ȶ�����
	delay();
	
	OUT_RELAY = 1;
		
	//��ʼ���ж�0
	initINT0();
	
	nrf24L01Init();
	
	nrfSetRxMode(); //��ʼ����
	
	while(1)
	{

	}
}

//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char* receivedData;
	unsigned char  senderID, param;
	
	receivedData = nrfGetReceivedData();
	
	//���ڵ����2���ֽ�
	//�ֽ�1��������ID  ��senderID��0ΪPi�������ڵ㣻senderID��111Ϊ�鷿�������ڵ㣩
	//�ֽ�2������ ���������£���
	//       0�ڵ㷢�͵Ĳ���Ϊ����ģʽ�� 0���ء�1������2���Զ�
	//       111�ڵ㷢�͵Ĳ���Ϊ���ص��źţ� 0���ء�1���� �����ڹ���ģʽΪ2ʱ��Ч��
	senderID = *receivedData++;
	param = *receivedData;
	
	if( senderID == 0 ) //���ڵ㷢��������
	{
		lightMode=param;
		if( lightMode == 0 )
		{
			OUT_RELAY=1;
		}
		else if( lightMode == 1 )
		{
			OUT_RELAY=0;
		}
	}
	else if( senderID==111 ) //�鷿����������������
	{
		if( lightMode == 2 )
		{
			//���յ�0��صƣ���0��򿪡�
			OUT_RELAY = (param==0)?1:0;
		}
	}

	//�ٴν������ģʽ
	//nrfSetRxMode();
}